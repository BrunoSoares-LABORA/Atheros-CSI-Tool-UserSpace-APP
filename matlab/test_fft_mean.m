function [mean, variance, last_index] = test_fft_mean(test_dir_prefix, num_traces)
    
    last_index = -1;
    for i = 1:num_traces
        sample_file = strcat(test_dir_prefix, '_', int2str(i));
        csi = subcarrier_pertime(sample_file, 1, 1);
        [csi_index, ~] = size(csi);
        if last_index == -1 || last_index > csi_index
            last_index = csi_index;
            if i > 1
                [T H N] = size(samples_data);
                for n = 1:N
                    aux_samples(:,:,n) = samples_data([1:last_index],:,n);
                end
                samples_data = aux_samples;
            end
        end
        csi_adjusted = csi([1:last_index], :);
        csi_fft = fft(csi_adjusted);
        samples_data(:,:,i) = csi_fft;
    end
    real_mean = sum(real(samples_data), 3)/num_traces;
    imag_mean = sum(imag(samples_data), 3)/num_traces;
    mean = complex(real_mean, imag_mean);
    
    real_variance = std(real(samples_data), 0, 3);
    imag_variance = std(imag(samples_data), 0, 3);
    variance = complex(real_variance, imag_variance);
end

