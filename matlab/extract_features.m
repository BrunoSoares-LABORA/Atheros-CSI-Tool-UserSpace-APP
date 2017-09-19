function [features] = extract_features(csi_matrix)
    function [f] = calcule_features(curve)
        c_mean = mean(curve);
        c_std = std(curve);
        c_entropy = entropy(curve);
        f = [c_mean, c_std, c_entropy];
    end
    
    treated_data = abs(csi_matrix.');
%     treated_data = pca_filter(treated_data, 2:6);
    treated_data = butter_filter(treated_data);
    
    % Extract features for each csi stream
    for i=[1]
        csi_stream_data = treated_data(:, i).';
        % Get wavelet curves
        [ t1, d1 ] = dwt( csi_stream_data, 'sym1', 'mode', 'sym' );
        [ t2, d2 ] = dwt( d1, 'sym1', 'mode', 'sym' );
        
%         figure;
%         subplot(3,2,1); plot(csi_stream_data); title('Original Signal');% axis([0 length(s) -5 5]);
%         subplot(3,2,3); plot(t1); title('Trend: Level 1');% axis([0 length(t1) -5 5]);
%         subplot(3,2,4); plot(d1); title('Detail: Level 1');% axis([0 length(d1) -5 5]);
%         subplot(3,2,5); plot(t2); title('Trend: Level 2');% axis([0 length(t2) -5 5]);
%         subplot(3,2,6); plot(d2); title('Detail: Level 2');%  axis([0 length(d2) -5 5]);

        % Extract feature vector
        fv = {csi_stream_data, t1, d1};
        fv = cellfun(@calcule_features, fv, 'UniformOutput', false);
        fv = cell2mat(fv);
        features{1,i} = fv;
    end
    features = cell2mat(features);
end