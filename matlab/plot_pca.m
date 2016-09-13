function [] = plot_pca(with_hand, without_hand)
    all_abs_data = [with_hand.abs; without_hand.abs];
    [wcoeff_all, score_all] = pca(all_abs_data);
    [wcoeff_with, score_with] = pca(with_hand.abs);
    [wcoeff_without, score_without] = pca(without_hand.abs);
    
    scorex = [min([min(score_all(:,1)), min(score_with(:,1)), min(score_without(:,1))]), max([max(score_all(:,1)), max(score_with(:,1)), max(score_without(:,1))])];
    scorey = [min([min(score_all(:,2)), min(score_with(:,2)), min(score_without(:,2))]), max([max(score_all(:,2)), max(score_with(:,2)), max(score_without(:,2))])];
    
    subplot(3,1,1);
    plot(score_all(:,1), score_all(:,2),'+')
    title('Scores containing both data');
    xlim(scorex)
    ylim(scorey)
    
    subplot(3,1,2);
    plot(score_with(:,1), score_with(:,2),'+')
    title('Scores with hand');
    xlim(scorex)
    ylim(scorey)
    
    subplot(3,1,3);
    plot(score_without(:,1), score_without(:,2),'+')
    title('Scores without hand');
    xlim(scorex)
    ylim(scorey)
    return;
    
    % plot pca
    coefforth_all = inv(diag(std(all_abs_data)))*wcoeff_all;
    coefforth_with = inv(diag(std(with_hand.abs)))*wcoeff_with;
    coefforth_without = inv(diag(std(without_hand.abs)))*wcoeff_without;
    
    subplot(3,1,1)
    biplot(coefforth_all(:,1:2), 'scores', score_all(:,1:2))
    title('PCA containing both data');
    xlim([-0.8 0.8]);
    ylim([-0.8 0.8]);
    
    subplot(3,1,2)
    biplot(coefforth_with(:,1:2), 'scores', score_with(:,1:2))
    title('PCA with hand');
    xlim([-0.8 0.8]);
    ylim([-0.8 0.8]);
    
    subplot(3,1,3)
    biplot(coefforth_without(:,1:2), 'scores', score_without(:,1:2))
    title('PCA without hand');
    xlim([-0.8 0.8]);
    ylim([-0.8 0.8]);
end

