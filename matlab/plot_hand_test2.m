function [] = plot_hand_test2(rx, tx)
    Fs = 1600;
    without_hand = test_fft_mean('/home/bruno/aedes_directional_clean-bottle_5-2', 1, rx, tx);
    disp('1 read');
    with_hand = test_fft_mean('/home/bruno/aedes_directional_fly_5-3', 1, rx, tx);
    disp('2 read');
    
    % define X and Y global
    for subcarrier =  1:56
        %without hand
        L = without_hand.last_index;
        without_abs = without_hand.abs([1:L/2+1], subcarrier);
        without_abs([2:end-1]) = 2*without_abs([2:end-1]);
        
        %with hand
        L = with_hand.last_index;
        with_abs = with_hand.abs([1:L/2+1], subcarrier);
        with_abs([2:end-1]) = 2*with_abs([2:end-1]);
        
        abs_min(subcarrier) = min([min(without_abs), min(with_abs)]);
        abs_max(subcarrier) = max([max(without_abs), max(with_abs)]);
        
        original_abs_without = without_hand.original_abs(:,subcarrier);
        original_abs_with = with_hand.original_abs(:,subcarrier);
        oabs_min(subcarrier) = min([min(original_abs_without), min(original_abs_with)]);
        oabs_max(subcarrier) = max([max(original_abs_without), max(original_abs_with)]);
    end
    
    absy = [min(abs_min), max(abs_max)];
    oabsy = [min(oabs_min), max(oabs_max)];
    
    for subcarrier = 1:56
        %without hand
        L = without_hand.last_index;
        without_abs = without_hand.abs([1:L/2+1], subcarrier);
        without_abs([2:end-1]) = 2*without_abs([2:end-1]);
        original_abs_without = without_hand.original_abs(:,subcarrier);
        
        %with hand
        L = with_hand.last_index;
        with_abs = with_hand.abs([1:L/2+1], subcarrier);
        with_abs([2:end-1]) = 2*with_abs([2:end-1]);
        original_abs_with = with_hand.original_abs(:,subcarrier);

        L = without_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(2,2,1);
        plot(f, without_abs);
        title(strcat('FFT ABS (Voltagem): 5GHz Direcional garrafa vazia - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);
        xlim([0, Fs/2]);

        subplot(2,2,3);
        plot(original_abs_without);
        title(strcat('ABS (Voltagem): 5GHz Direcional garrafa vazia - Portadora:', int2str(subcarrier)));
        xlabel('Pacote (N)');
        ylabel('Volts (Média)');
        ylim(oabsy);
        xlim([0, L]);

        % with hand
        L = with_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(2,2,2);
        plot(f, with_abs);
        title(strcat('FFT ABS (Voltagem): 5GHz Direcional com mosquito - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);
        xlim([0, Fs/2]);

        subplot(2,2,4);
        plot(original_abs_with);
        title(strcat('ABS (Voltagem): 5GHz Direcional com mosquito - Portadora:', int2str(subcarrier)));
        xlabel('Pacote (N)');
        ylabel('Volts (Média)');
        ylim(oabsy);
        xlim([0, L]);
        
        set(gcf, 'PaperUnits', 'inches', 'PaperPosition', [0 0 21.3 10.6])
        filename = strcat('/home/bruno/matlabout/', int2str(rx), '-', int2str(tx), '_', int2str(subcarrier));
        print(filename,'-dpng','-r0');
    end
end