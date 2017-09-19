close all;

%clean_trace = read_log_file('/home/bruno/csi-tests/sitting/person_in_lab_clean_1');
% clean_trace = read_log_file('/home/bruno/csi-tests/moving/person_in_lab_without_movement_3');
%clean_trace = read_log_file('/home/bruno/csi-tests/falling/person_in_lab_clean_1');
% clean_trace = get_csi_streams(clean_trace, 1, 1);

%event_trace = read_log_file('/home/bruno/csi-tests/sitting/person_in_lab_sitting_1');
% event_trace = read_log_file('/home/bruno/csi-tests/moving/person_in_lab_with_movement_1');
%event_trace = read_log_file('/home/bruno/csi-tests/falling/person_in_lab_falling_1');
% event_trace = get_csi_streams(event_trace, 1, 1);

X = abs(clean_trace.');
Y = abs(event_trace.');

stream_id = 1;
to = 1;
tf = 7700;
sc = 2:6;

x = pca_filter(X, sc);
y = pca_filter(Y, sc);

figure;
subplot(2,3,1), plot( X(to:tf,stream_id) ); title('Sinal sem pessoa andando na sala - SEM FILTRO');
subplot(2,3,2), plot( x(to:tf,stream_id) ); title('Sinal sem pessoa andando na sala - PCA');
subplot(2,3,3), plot( butter_filter(X(to:tf,stream_id)) );  title('Sinal sem pessoa andando na sala - Butter');
subplot(2,3,4), plot( Y(to:tf,stream_id) ); title('Sinal com pessoa andando na sala - SEM FILTRO');
subplot(2,3,5), plot( y(to:tf,stream_id) ); title('Sinal com pessoa andando na sala - PCA');
subplot(2,3,6), plot( butter_filter(Y(to:tf,stream_id)) );  title('Sinal com pessoa andando na sala - Butter');

figure
fft_data = abs(fft(y(to:tf,stream_id)));
Fs = 800;
[L, ~] = size(y);
fft_plot = fft_data([1:L/2+1]);
fft_plot([2:end-1]) = 2*fft_plot([2:end-1]);
f = Fs*(0:(L/2))/L;
plot(f, fft_plot);

figure;
subplot(2,1,1); spectrogram(x(to:tf,1), 400, 200, 400, 800); title('Spectrograma sem');
subplot(2,1,2); spectrogram(y(to:tf,1), 400, 200, 400, 800); title('Spectrograma com');


% % Codigo MATLAB. Autor: Gustavo Teodoro Laureano
s = y(:,stream_id);
%s = sumsin*1.5; % Hypothetical signal

[ t1, d1 ] = dwt( s, 'sym1', 'mode', 'sym' );
[ t2, d2 ] = dwt( t1, 'sym1', 'mode', 'sym' );
[ t3, d3 ] = dwt( t2, 'sym1', 'mode', 'sym' );
[ t4, d4 ] = dwt( t3, 'sym1', 'mode', 'sym' );
[ t5, d5 ] = dwt( t4, 'sym1', 'mode', 'sym' );
[ t6, d6 ] = dwt( t5, 'sym1', 'mode', 'sym' );
[ t7, d7 ] = dwt( t6, 'sym1', 'mode', 'sym' );

figure;
subplot(8,2,1); plot(s); title('Original Signal');% axis([0 length(s) -5 5]);
subplot(8,2,3); plot(t1); title('Trend: Level 1');% axis([0 length(t1) -5 5]);
subplot(8,2,4); plot(d1); title('Detail: Level 1');% axis([0 length(d1) -5 5]);
subplot(8,2,5); plot(t2); title('Trend: Level 2');% axis([0 length(t2) -5 5]);
subplot(8,2,6); plot(d2); title('Detail: Level 2');%  axis([0 length(d2) -5 5]);
subplot(8,2,7); plot(t3); title('Trend: Level 3');%  axis([0 length(t3) -5 5]);
subplot(8,2,8); plot(d3); title('Detail: Level 3');%  axis([0 length(d3) -5 5]);
subplot(8,2,9); plot(t4); title('Trend: Level 4');%  axis([0 length(t4) -5 5]);
subplot(8,2,10); plot(d4); title('Detail: Level 4');%  axis([0 length(d4) -5 5]);
subplot(8,2,11); plot(t5); title('Trend: Level 5');%  axis([0 length(t4) -5 5]);
subplot(8,2,12); plot(d5); title('Detail: Level 5');%  axis([0 length(d4) -5 5]);
subplot(8,2,13); plot(t6); title('Trend: Level 6');%  axis([0 length(t4) -5 5]);
subplot(8,2,14); plot(d6); title('Detail: Level 6');%  axis([0 length(d4) -5 5]);
subplot(8,2,15); plot(t7); title('Trend: Level 7');%  axis([0 length(t4) -5 5]);
subplot(8,2,16); plot(d7); title('Detail: Level 7');%  axis([0 length(d4) -5 5]);

%axis([0 16000 -5 5]);
