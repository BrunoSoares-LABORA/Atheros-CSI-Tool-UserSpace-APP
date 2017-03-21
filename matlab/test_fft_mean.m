function [x] = test_fft_mean(test_dir_prefix, num_traces, rx, tx)    
    last_index = -1;
    for i = 1:num_traces
        sample_file = strcat(test_dir_prefix, '_', int2str(i));
        atheros_csi = read_log_file(sample_file);
        csi = subcarrier_pertime(atheros_csi);
        csi = squeeze(csi(rx,tx,:,:)).';
        
        [csi_index, ~] = size(csi);
        if last_index == -1 || last_index > csi_index
            if last_index == -1
                max_data = round(csi_index * 1.2);
            end
            last_index = csi_index;
        end
        csi_adjusted = csi([1:last_index], :);
        csi_adjusted([end+1:max_data],:) = zeros('like', csi_adjusted);
        samples_data(:,:,i) = csi_adjusted;
    end
    
    samples_data = samples_data([1:last_index],:,:);
    x = samples_data;
    return;
    
    abs_fft = fft(abs(samples_data));
    angle_fft = fft(angle(samples_data));
    csi_angle_fft = fft(csi_angle(samples_data));
    real_fft = fft(real(samples_data));
    imag_fft = fft(imag(samples_data));
    
    abs_data = abs(abs_fft) / last_index;
    abs_r = sum(abs_data, 3) / num_traces;
    abs_r(1,:) = 0;
    
    angle_data = abs(angle_fft) / last_index;
    angle_r = sum(angle_data, 3) / num_traces;
    csi_angle_data = abs(csi_angle_fft) / last_index;
    csi_angle_r = sum(csi_angle_data, 3) / num_traces;
    
    real_data = abs(real_fft) / last_index;
    real_r = sum(real_data, 3) / num_traces;
    imag_data = abs(imag_fft) / last_index;
    imag_r = sum(imag_data, 3) / num_traces;
    
    original_abs = abs(samples_data);
    original_abs_r = sum(original_abs, 3) / num_traces;
    
    x = struct();
    x.abs = abs_r;
    x.angle = angle_r;
    x.csi_angle = csi_angle_r;
    x.real = real_r;
    x.imag = imag_r;
    x.last_index = last_index;
    x.original_abs = original_abs_r;
end