function [] = plot_time_hand_test(rx, tx)
    without_hand = test_time_mean('sample/hand_test_1000/hand_test_without_hand_1000_1', rx, tx);
    with_hand = test_time_mean('sample/hand_test_1000/hand_test_with_hand_1000_1', rx, tx);
    
    for i = 1:2
        if i == 1
            test = without_hand;
            concat = 'Sem';
            prefix = 'without';
        else
            test = with_hand;
            concat = 'Com';
            prefix = 'with';
        end
        
        for subcarrier = 1:56
            plot(test.o_m_real_data(subcarrier,[1:10000]).', 'black');
            hold on
            plot(linspace(1, 10000, 1000), test.csec_m_real_data(subcarrier,[1:1000]).', 'green');
            hold on
            plot(linspace(1, 10000, 100), test.dsec_m_real_data(subcarrier,[1:100]).', 'red');
            hold on
            errorbar(linspace(1, 10000, 10), test.sec_m_real_data(subcarrier,[1:10]).', test.sec_s_real_data(subcarrier,[1:10]).', 'blue');
            hold off

            set(findobj(gca, 'Type', 'Line', 'Color', 'red'), 'LineWidth', 3);
            set(findobj(gca, 'Type', 'Line', 'Color', 'blue'), 'LineWidth', 2);
            title(strcat('Real: ', concat, ' mão - Portadora:', int2str(subcarrier)));
            xlabel('t(ms)');
            ylabel('r');
            ylim([-1000 1000]);
            xlim([0, 10000]);
            set(gcf,'PaperUnits','inches','PaperPosition',[0 0 16.6 7.7])
            filename = strcat('/home/bruno/matlabout/', prefix, '_', int2str(rx), '-', int2str(tx), '_', int2str(subcarrier));
            print(filename,'-dpng','-r0');
        end
    end
end

