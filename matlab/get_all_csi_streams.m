function data = get_all_csi_streams(csi)
    atheros_csi = csi.';
    n_traces = size(atheros_csi);
    data = struct();
    for index = 1:n_traces(1,2)
        csi_data = atheros_csi{1,index}.csi;
        % gets rx and tx subcarriers data
        [M, N, S] = size(csi_data);
        field_name = strcat('a', num2str(M), num2str(N));
        csi_data_in_vector = reshape(csi_data, 1, numel(csi_data));
        
        if isfield(data, field_name) == 1
            all_data = getfield(data, field_name);
            all_data = cat(1, csi_data_in_vector, all_data);
        else
            all_data = csi_data_in_vector;
        end
        
        data = setfield(data, field_name, all_data);
    end
end