function [mean, variance, last_index] = test_fft_mean(test_dir_prefix, num_traces, rx, tx)
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
    
    real_mean = sum(real(samples_data), 3)/num_traces;
    imag_mean = sum(imag(samples_data), 3)/num_traces;
    mean = complex(real_mean, imag_mean);
    
    real_variance = std(real(samples_data), 0, 3);
    imag_variance = std(imag(samples_data), 0, 3);
    variance = complex(real_variance, imag_variance);
end

