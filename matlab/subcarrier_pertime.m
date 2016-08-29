function subcarriers=subcarrier_pertime(log_file_path, rx, tx)
    atheros_csi = read_log_file(log_file_path).';
    subcarriers = zeros(1,atheros_csi{1,1}.num_tones);
    n_traces = size(atheros_csi);
    for index = 1:n_traces(1,2)
        csi_data = atheros_csi{1,index}.csi;
        
        % gets rx and tx subcarriers data
        [M N ~] = size(csi_data);
        if M >= rx && N >= tx
            csi_data = csi_data(rx,tx,:);
            csi_data = squeeze(csi_data).';
            subcarriers(index,:) = csi_data(1,:);
        end
    end
end