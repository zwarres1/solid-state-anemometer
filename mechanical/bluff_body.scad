$fa = 1;
$fs = 0.4;

// --- Parameters ---
height = 60;
base_size = 50;
steps = 60;
decay_rate = 0.04;

module curved_square_solid() {
    step_h = height / steps;
    
    for (i = [0 : steps - 1]) {
        // Absolute scales for the bottom and top of this specific step
        scale_current = exp(-i * decay_rate);
        scale_next = exp(-(i + 1) * decay_rate);
        
        // Relative scale change for this specific extrusion
        step_scale = scale_next / scale_current;
        
        translate([0, 0, i * step_h])
            linear_extrude(height = step_h, scale = step_scale)
                square([base_size * scale_current, base_size * scale_current], center=true);
    }
}

curved_square_solid();