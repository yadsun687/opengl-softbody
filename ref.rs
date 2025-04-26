// viscosity force
#[inline(always)]
    fn compute_viscosity_term(
        masses: &[f32],
        densities: &[f32],
        velocities: &[nalgebra::SVector<f32, D>],
        positions: &[nalgebra::Point<f32, D>],
        spartial_index: &SpatialIndex,
        i: usize,
        mu: f32,
        h: f32,
        dw_dr: impl Fn(f32, f32) -> f32 + Send + Sync,
    ) -> nalgebra::SVector<f32, D> {
        let mut sum = nalgebra::SVector::zeros();
        let pos_i = positions[i];
        let v_i = velocities[i];

        for j in spartial_index.get_neighbors(pos_i, &positions) {
            let pos_j = positions[j];
            let v_j = velocities[j];
            
            let r = pos_i - pos_j;
            let r_norm = r.norm();

            if r_norm <= 0.000001 {
                continue;
            }

            sum += (masses[j]/densities[j]) * (v_i - v_j) * 2.0 * (dw_dr(r_norm, h) / r_norm);
        }

        sum * mu / masses[i]
    }
// step
pub(crate) fn step_sph<S: Scene<D> + Send + Sync, const D: usize>(
    scene: &mut S,
    w: impl Fn(f32, f32) -> f32 + Send + Sync,
    grad_w: impl Fn(SVector<f32, D>, f32) -> SVector<f32, D> + Send + Sync,
    delta_t: f32,
    h: f32,
) {
    // calculate densities for all samples
    scene.recalculate_densities(w, h);

    let n = scene.positions().len();
    
    (0..scene.positions().len()).into_par_iter().for_each(|i| {
        let mut a: nalgebra::SVector<f32, D> = nalgebra::SVector::zeros();

        // compute pressure term        
        a += compute_pressure_term(scene, &grad_w, i, h);

        // TODO compute viscosity term
        
        // add external acceleration (gravity)
        a[1] += -G;

        // update velocity
        scene.update_velocity(i, a * delta_t);
    });
}

//pressure force
fn compute_pressure_term<S: Scene<D>, const D: usize>(
    scene: &S,
    grad_w: impl Fn(SVector<f32, D>, f32) -> SVector<f32, D>,
    i: usize,
    h: f32,
) -> nalgebra::SVector<f32, D> {
    let mut sum = nalgebra::SVector::zeros();

    let sample_i = scene.get_sample(i);
    let m_i = sample_i.mass;
    let rho_i = scene.get_density(i);
    let p_i = K * (rho_i - DENSITY_0);
    let x_i = p_i / (rho_i * rho_i);

    for (j, sample_j) in scene.enumerate_neighbors_within_radius_squared(&sample_i.pos, h * h) {
        let m_j = sample_j.mass;
        let rho_j = scene.get_density(j);
        let p_j = K * (rho_j - DENSITY_0);
        let x_j = p_j / (rho_j * rho_j);    
        
        let r = sample_j.pos - sample_i.pos;

        if r.norm() <= 0.000001 {
            continue;
        }

        sum += m_j * (x_i + x_j) * grad_w(r, h);
    }

    sum / rho_i
}

// kernel
pub(crate) mod cubic_spline {
    #[inline(always)]
    pub(crate) fn w<const D: usize>(r: f32, h: f32) -> f32 {
        let q = r / h;
        let factor = factor::<D>(h);
        if q <= 0.5 {
            factor * (6.0 * (q * q * q - q * q) + 1.0)
        }
        else if q <= 1.0 {
            factor * (2.0 * (1.0 - q) * (1.0 - q) * (1.0 - q))
        }
        else {
            0.0
        }
    }

    #[inline(always)]
    pub(crate) const fn factor<const D: usize>(h: f32) -> f32 {
        match D {
            1 => 4.0 / (3.0 * h),
            2 => 40.0 / (7.0 * std::f32::consts::PI * h * h),
            3 => 8.0 / (std::f32::consts::PI * h * h * h),
            _ => panic!("Unsupported dimension"),
        }
    }

    #[inline(always)]
    pub fn dw_dr<const D: usize>(r: f32, h: f32) -> f32 {
        let factor = factor::<D>(h);
        let q = r / h;
        
        if q < 0.5 {
            (6.0 * factor * r) / (h * h) * (3.0 * r / h - 2.0)
        } else if q < 1.0 {
            -6.0 * factor * (1.0 - q) * (1.0 - q) / h
        } else {
            0.0
        }
    }

    #[inline(always)]
    pub fn grad_w<const D: usize>(r_vec: nalgebra::SVector<f32, D>, h: f32) -> nalgebra::SVector<f32, D> {
        let r = r_vec.norm();
        let dw_dr = dw_dr::<D>(r, h);
        dw_dr * (r_vec / r)
    }
}

// density
fn compute_density<S: Scene<N, D>, const N: usize, const D: usize>(scene: &S, pos: &nalgebra::Point<f32, D>) -> f32 {
    let h = kernels::max_cubic_spline_kernel_radius(H);
    let mut density = 0.0;
    for neighbor in scene.iter_neighbors_within_radius_squared(pos, h * h * 4.0) {
        let r = nalgebra::distance(pos, &neighbor.pos);
        let w = kernels::cubic_spline_kernel(r, h);
        density += neighbor.mass * w;
    }
    density
}