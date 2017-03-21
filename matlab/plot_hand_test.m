function [] = plot_hand_test(rx, tx)
    Fs = 100;
    without_hand = test_fft_mean('/home/bruno/csi-tests/2.4_aedes_test', 1, rx, tx);
    disp('1 read');
    with_hand = test_fft_mean('/home/bruno/csi-tests/5_aedes_test', 1, rx, tx);
    disp('2 read');
    
    % define X and Y global
    for subcarrier =  1:56
        %without hand
        L = without_hand.last_index;
        without_abs = without_hand.abs([1:L/2+1], subcarrier);
        without_abs([2:end-1]) = 2*without_abs([2:end-1]);
        without_angle = without_hand.angle([1:L/2+1], subcarrier);
        without_angle([2:end-1]) = 2*without_angle([2:end-1]);
        without_csi_angle = without_hand.csi_angle([1:L/2+1], subcarrier);
        without_csi_angle([2:end-1]) = 2*without_csi_angle([2:end-1]);
        
        %with hand
        L = with_hand.last_index;
        with_abs = with_hand.abs([1:L/2+1], subcarrier);
        with_abs([2:end-1]) = 2*with_abs([2:end-1]);
        with_angle = with_hand.angle([1:L/2+1], subcarrier);
        with_angle([2:end-1]) = 2*with_angle([2:end-1]);
        with_csi_angle = with_hand.csi_angle([1:L/2+1], subcarrier);
        with_csi_angle([2:end-1]) = 2*with_csi_angle([2:end-1]);
        
        abs_min(subcarrier) = min([min(without_abs), min(with_abs)]);
        abs_max(subcarrier) = max([max(without_abs), max(with_abs)]);
        angle_min(subcarrier) = min([min(without_angle), min(with_angle)]);
        angle_max(subcarrier) = max([max(without_angle), max(with_angle)]);
        csi_angle_min(subcarrier) = min([min(without_csi_angle), min(with_csi_angle)]);
        csi_angle_max(subcarrier) = max([max(without_csi_angle), max(with_csi_angle)]);
    end
    
    absy = [min(abs_min), max(abs_max)];
    angley = [min(angle_min), max(angle_max)];
    csiangley = [min(csi_angle_min), max(csi_angle_max)];
    
    for subcarrier = 1:56
        %without hand
        L = without_hand.last_index;
        without_abs = without_hand.abs([1:L/2+1], subcarrier);
        without_abs([2:end-1]) = 2*without_abs([2:end-1]);
        without_angle = without_hand.angle([1:L/2+1], subcarrier);
        without_angle([2:end-1]) = 2*without_angle([2:end-1]);
        without_csi_angle = without_hand.csi_angle([1:L/2+1], subcarrier);
        without_csi_angle([2:end-1]) = 2*without_csi_angle([2:end-1]);
        
        %with hand
        L = with_hand.last_index;
        with_abs = with_hand.abs([1:L/2+1], subcarrier);
        with_abs([2:end-1]) = 2*with_abs([2:end-1]);
        with_angle = with_hand.angle([1:L/2+1], subcarrier);
        with_angle([2:end-1]) = 2*with_angle([2:end-1]);
        with_csi_angle = with_hand.csi_angle([1:L/2+1], subcarrier);
        with_csi_angle([2:end-1]) = 2*with_csi_angle([2:end-1]);

        L = without_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(3,2,1);
        plot(f, without_abs);
        title(strcat('ABS (Voltagem): Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);

        subplot(3,2,3);
        plot(f, without_angle);
        title(strcat('ANGLE (MATLAB): Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(angley);
        
        subplot(3,2,5);
        plot(f, without_csi_angle);
        title(strcat('ANGLE (CSI): Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(csiangley);

        % with hand
        L = with_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(3,2,2);
        plot(f, with_abs);
        title(strcat('ABS (Voltagem): Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);

        subplot(3,2,4);
        plot(f, with_angle);
        title(strcat('ANGLE (MATLAB): Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(angley);

        subplot(3,2,6);
        plot(f, with_csi_angle);
        title(strcat('ANGLE (CSI): Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(csiangley);
        
        set(gcf, 'PaperUnits', 'inches', 'PaperPosition', [0 0 21.3 10.6])
        filename = strcat('/home/bruno/matlabout/', int2str(rx), '-', int2str(tx), '_', int2str(subcarrier));
        print(filename,'-dpng','-r0');
    end
end