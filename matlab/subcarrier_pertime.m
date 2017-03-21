function subcarriers = subcarrier_pertime(csi)
    atheros_csi = csi.';
    num_tones = atheros_csi{1,1}.num_tones;
    n_traces = size(atheros_csi);
    for index = 1:n_traces(1,2)
        csi_data = atheros_csi{1,index}.csi;
        % gets rx and tx subcarriers data
        [M, N, S] = size(csi_data);
        if S == num_tones && M == 3 && N == 3
            if exist('subcarriers', 'var')
                subcarriers(:,:,:,end+1) = csi_data;
            else
                subcarriers(:,:,:,1) = csi_data;
            end
        end
    end
end