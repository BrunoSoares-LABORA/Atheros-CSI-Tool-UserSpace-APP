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
        csi_fft = fft(csi_adjusted);
        samples_data(:,:,i) = csi_fft;
    end
    
    samples_data = samples_data([1:last_index],:,:);
    
    real_data = real(samples_data);
    real_r = sum(real_data, 3) / num_traces;
    abs_data = abs(samples_data/last_index);
    abs_r = sum(abs_data, 3) / num_traces;
    angle_data = angle(samples_data);
    angle_r = sum(angle_data, 3) / num_traces;
    
    x = struct();
    x.abs = abs_r;
    x.real = real_r;
    x.angle = angle_r;
    x.last_index = last_index;
end