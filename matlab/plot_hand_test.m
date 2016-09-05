function [] = plot_hand_test(rx, tx)
    Fs = 1000;
    without_hand = test_fft_mean('sample/hand_test_1000/without_hand_1000', 30, rx, tx);
    with_hand = test_fft_mean('sample/hand_test_1000/with_hand_1000', 30, rx, tx);
    
    for subcarrier = 1:56
        %without hand
        L = without_hand.last_index;
        without_abs = without_hand.abs([1:L/2+1], subcarrier);
        without_abs([2:end-1]) = 2*without_abs([2:end-1]);
        without_real = without_hand.real([1:L/2+1], subcarrier);
        without_real([2:end-1]) = 2*without_real([2:end-1]);
        without_angle = without_hand.angle([1:L/2+1], subcarrier);
        without_angle([2:end-1]) = 2*without_angle([2:end-1]);
        
        %with hand
        L = with_hand.last_index;
        with_abs = with_hand.abs([1:L/2+1], subcarrier);
        with_abs([2:end-1]) = 2*with_abs([2:end-1]);
        with_real = with_hand.real([1:L/2+1], subcarrier);
        with_real([2:end-1]) = 2*with_real([2:end-1]);
        with_angle = with_hand.angle([1:L/2+1], subcarrier);
        with_angle([2:end-1]) = 2*with_angle([2:end-1]);
        
        absy = [min([min(without_abs), min(with_abs)]), max([max(without_abs), max(with_abs)])];
        realy = [min([min(without_real), min(with_real)]), max([max(without_real), max(with_real)])];
        angley = [min([min(without_angle), min(with_angle)]), max([max(without_angle), max(with_angle)])];

        L = without_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(3,2,1);
        plot(f, without_abs);
        title(strcat('ABS: Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);

        subplot(3,2,3);
        plot(f, without_real);
        title(strcat('REAL: Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(realy);

        subplot(3,2,5);
        plot(f, without_angle);
        title(strcat('ANGLE: Sem mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(angley);

        % with hand
        L = with_hand.last_index;
        f = Fs*(0:(L/2))/L;
        subplot(3,2,2);
        plot(f, with_abs);
        title(strcat('ABS: Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(absy);

        subplot(3,2,4);
        plot(f, with_real);
        title(strcat('REAL: Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(realy);

        subplot(3,2,6);
        plot(f, with_angle);
        title(strcat('ANGLE: Com mão - Portadora:', int2str(subcarrier)));
        xlabel('f (Hz)');
        ylabel('|P1(f)|');
        ylim(angley);
        
        set(gcf, 'PaperUnits', 'inches', 'PaperPosition', [0 0 21.3 10.6])
        filename = strcat('/home/bruno/matlabout/', int2str(rx), '-', int2str(tx), '_', int2str(subcarrier));
        print(filename,'-dpng','-r0');
    end
end