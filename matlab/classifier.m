% % Convert old data format to the new second format
% x = bruno_static_heart;
% for i=1:size(x,1)
%     for j=1:size(x,3)/25
%         if(exist('y') == 0)
%             y(1,:,:) = x(i,:,(25*(j-1))+1:25*j);
%         else
%             y(end+1,:,:) = x(i,:,(25*(j-1))+1:25*j);
%         end
%     end
% end
% 
% 

% Get from realtime_view (data(end+1,:,:) = csi_data(:,1:100);)
%data = cat(1,csi_clear_lab_by_sec,csi_bruno_by_sec);
data = csi_clear_lab_by_sec;
labels = cell(size(data,1), 1);
labels(1:52,:) = {'Limpo'};
%labels(53:132,:) = {'Bruno'};

% Compute feature vector for all experiments
for i=1:size(data,1)
    trace_data = kICA(squeeze(data(i,:,:)).', 1);
    feature_vector(i,:) = extract_features(trace_data);
end

clear treated_data i;

% Create Naive Bayes classifier
X = feature_vector;
y = labels;

% Set the mean features as a kernel distribution because it can be negative
distributions = cell(1, size(feature_vector,2));
distributions(:) = {'kernel'};
indices = 1:3:size(distributions,2);
distributions(:,indices) = {'kernel'};

NBModel1 = fitNaiveBayes(X,y, 'Distribution', distributions);
NBModel1.ClassLevels

%Predict some data
predictLabels1 = predict(NBModel1,X);
[ConfusionMat1, NBlabels] = confusionmat(y,predictLabels1);
disp(ConfusionMat1);

clear feature_vector


