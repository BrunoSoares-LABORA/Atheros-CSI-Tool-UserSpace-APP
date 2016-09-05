function [x] = test_time_mean( file_path, rx, tx )
    csi = read_log_file(file_path);
    csi_data = subcarrier_pertime(csi, rx, tx);
    
    [csi_size, ~] = size(csi);
    [csi_data_size, ~] = size(csi_data);
    if csi_size ~= csi_data_size
        disp('Could not print csi time mean');
        return;
    end
    
    first = csi{1,1}.timestamp;
    last = csi{end,1}.timestamp;
    first_sec = floor(first / 10^6);
    first_dsec = floor(first / 10^5);
    first_csec = floor(first / 10^4);
    last_sec = floor(last / 10^6);
    last_dsec = floor(last / 10^5);
    last_csec = floor(last / 10^4);
    
    sec_data = cell(1, last_sec-first_sec+1);
    dsec_data = cell(1, last_dsec-first_dsec+1);
    csec_data = cell(1, last_csec-first_csec+1);
    
    for i = 1:csi_size
        trace_time = csi{i,1}.timestamp;
        sec = floor(trace_time / 10^6) - first_sec + 1;
        dsec = floor(trace_time / 10^5) - first_dsec + 1;
        csec = floor(trace_time / 10^4) - first_csec + 1;
        
        sec_data{sec}(end+1,:) = csi_data(i,:);
        dsec_data{dsec}(end+1,:) = csi_data(i,:);
        csec_data{csec}(end+1,:) = csi_data(i,:);
    end
    
    % compute mean and std
    for i = 1:last_sec-first_sec+1
        compute_data = sec_data{i};
        [N ~] = size(compute_data);
        
        if N > 0
            real_data = real(compute_data);
            sec_m_real_data(:,i) = sum(real_data, 1) / N;
            sec_s_real_data(:,i) = std(real_data, 0, 1);

            abs_data = abs(compute_data);
            sec_m_abs_data(:,i) = sum(abs_data, 1) / N;
            sec_s_abs_data(:,i) = std(abs_data, 0, 1);

            angle_data = angle(compute_data);
            sec_m_angle_data(:,i) = sum(angle_data, 1) / N;
            sec_s_angle_data(:,i) = std(angle_data, 0, 1);
        end
    end
    
    for i = 1:last_dsec-first_dsec+1
        compute_data = dsec_data{i};
        [N ~] = size(compute_data);
        
        if N > 0
            real_data = real(compute_data);
            dsec_m_real_data(:,i) = sum(real_data, 1) / N;
            dsec_s_real_data(:,i) = std(real_data, 0, 1);

            abs_data = abs(compute_data);
            dsec_m_abs_data(:,i) = sum(abs_data, 1) / N;
            dsec_s_abs_data(:,i) = std(abs_data, 0, 1);

            angle_data = angle(compute_data);
            dsec_m_angle_data(:,i) = sum(angle_data, 1) / N;
            dsec_s_angle_data(:,i) = std(angle_data, 0, 1);
        end
    end
    
    for i = 1:last_csec-first_csec+1
        compute_data = csec_data{i};
        [N ~] = size(compute_data);
        
        if N > 0
            real_data = real(compute_data);
            csec_m_real_data(:,i) = sum(real_data, 1) / N;
            csec_s_real_data(:,i) = std(real_data, 0, 1);

            abs_data = abs(compute_data);
            csec_m_abs_data(:,i) = sum(abs_data, 1) / N;
            csec_s_abs_data(:,i) = std(abs_data, 0, 1);

            angle_data = angle(compute_data);
            csec_m_angle_data(:,i) = sum(angle_data, 1) / N;
            csec_s_angle_data(:,i) = std(angle_data, 0, 1);
        end
    end
    
    original_data = csi_data.';
    
    x = struct();
    x.sec_m_real_data = sec_m_real_data;
    x.sec_m_abs_data = sec_m_abs_data;
    x.sec_m_angle_data = sec_m_angle_data;
    x.sec_s_real_data = sec_s_real_data;
    x.sec_s_abs_data = sec_s_abs_data;
    x.sec_s_angle_data = sec_s_angle_data;
    x.dsec_m_real_data = dsec_m_real_data;
    x.dsec_m_abs_data = dsec_m_abs_data;
    x.dsec_m_angle_data = dsec_m_angle_data;
    x.dsec_s_real_data = dsec_s_real_data;
    x.dsec_s_abs_data = dsec_s_abs_data;
    x.dsec_s_angle_data = dsec_s_angle_data;
    x.csec_m_real_data = csec_m_real_data;
    x.csec_m_abs_data = csec_m_abs_data;
    x.csec_m_angle_data = csec_m_angle_data;
    x.csec_s_real_data = csec_s_real_data;
    x.csec_s_abs_data = csec_s_abs_data;
    x.csec_s_angle_data = csec_s_angle_data;
    x.o_m_real_data = real(original_data);
    x.o_s_real_data = std(real(original_data), 0, 1);
    x.o_m_abs_data = abs(original_data);
    x.o_s_abs_data = std(abs(original_data), 0, 1);
    x.o_m_angle_data = angle(original_data);
    x.o_s_angle_data = std(angle(original_data), 0, 1);
end

