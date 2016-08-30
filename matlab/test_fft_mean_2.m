function [abs_r, real_r, angle_r, last_index] = test_fft_mean_2(test_dir_prefix, num_traces, rx, tx)
    frequency = 100;
    lenght = 30;
    max_data = (lenght + 2) * frequency;
    
    last_index = -1;
    for i = 1:num_traces
        sample_file = strcat(test_dir_prefix, '_', int2str(i));
        csi = subcarrier_pertime(sample_file, rx, tx);
        [csi_index, ~] = size(csi);
        if last_index == -1 || last_index > csi_index
            last_index = csi_index;
        end
        csi_adjusted = csi([1:last_index], :);
        csi_adjusted([end+1:max_data],:) = zeros('like', csi_adjusted);
        csi_fft = fft(csi_adjusted);
        samples_data(:,:,i) = csi_fft;
    end
    
    max_useful = floor(last_index/frequency) * frequency;
    samples_data = samples_data([1:max_useful],:,:);
    
    real_data = real(samples_data);
    real_r = sum(real_data, 3) / num_traces;
    abs_data = abs(samples_data/lenght);
    abs_r = sum(abs_data, 3) / num_traces;
    angle_data = angle(samples_data);
    angle_r = sum(angle_data, 3) / num_traces;
end