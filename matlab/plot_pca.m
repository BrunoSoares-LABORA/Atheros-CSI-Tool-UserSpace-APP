function [] = plot_pca(rx, tx)
    without_hand = test_fft_mean('sample/hand_test_1000/without_hand_1000', 30, rx, tx);
    with_hand = test_fft_mean('sample/hand_test_1000/with_hand_1000', 30, rx, tx);
    
    all_abs_data = [with_hand.abs; without_hand.abs];
    [wcoeff_all, score_all] = pca(all_abs_data);
    
    scorex = [min(score_all(:,1)), max(score_all(:,1))];
    scorey = [min(score_all(:,2)), max(score_all(:,2))];
    
    subplot(3,1,1);
    plot(score_all(:,1), score_all(:,2),'+')
    title('Scores containing both data');
    xlim(scorex)
    ylim(scorey)
    
    subplot(3,1,2);
    plot(score_all([1:with_hand.last_index],1), score_all([1:with_hand.last_index],2),'+')
    title('Scores with hand');
    xlim(scorex)
    ylim(scorey)
    
    subplot(3,1,3);
    plot(score_all([(with_hand.last_index + 1):end],1), score_all([(with_hand.last_index + 1):end],2),'+')
    title('Scores without hand');
    xlim(scorex)
    ylim(scorey)
end

