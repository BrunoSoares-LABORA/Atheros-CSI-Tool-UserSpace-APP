function [] = plot_csi_trace_periodicity(file_path)
    csi = read_log_file(file_path);
    
    [csi_size, ~] = size(csi);
    for i = 1:csi_size
        if i == 1
            time_periodicity(1,1) = 0;
            last_time = csi{i,1}.timestamp;
            continue
        end
        
        trace_time = csi{i,1}.timestamp;
        time_periodicity(1,end+1) = trace_time - last_time;
        last_time = trace_time;
    end
    
    plot(time_periodicity);
    xlabel('packet number');
    ylabel('t(us)');
    title('Packet interval');
    xlim([0 10000]);
    ylim([0 10000]);
end

