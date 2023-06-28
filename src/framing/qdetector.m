clear all;
close all;
f = figure();
pos = get(f, 'position');
set(f, 'position', [pos(1) pos(2) 1400 1050]);
set(f, 'color', get(0, 'defaultuicontrolbackgroundcolor'));

function update_phase_offset(obj)
    inst = guidata(obj);
    switch(obj)
        case {inst.phase_offset_edit}
            inst.rxy_index = str2num(get(obj, 'string')) + 1;
        case {inst.phase_offset_minus_button}
            inst.rxy_index = inst.rxy_index - 1;
        case {inst.phase_offset_plus_button}
            inst.rxy_index = inst.rxy_index + 1;
    endswitch

    inst.rxy_index = max(1, min(length(inst.selected_rxy), inst.rxy_index));
    inst = render(inst);
    guidata(obj, inst);
endfunction

function update_freq_offset(obj)
    inst = guidata(obj);
    switch(obj)
        case {inst.correlation_offset_edit}
            inst.peak_offset = str2num(get(obj, 'string')) + floor(inst.rxy_length / 2) + 1;
        case {inst.correlation_offset_minus_button}
            inst.peak_offset = inst.peak_offset - 1;
        case {inst.correlation_offset_plus_button}
            inst.peak_offset = inst.peak_offset + 1;
    endswitch

    inst.peak_offset = max(1, min(size(inst.rxy, 1), inst.peak_offset));

    inst.selected_rxy = inst.rxy(inst.peak_offset,:);
    [inst.freq_peak_corr inst.freq_peak_offset] = max(inst.selected_rxy);
    inst.rxy_index = inst.freq_peak_offset;
    inst = render(inst);
    guidata(obj, inst);
endfunction

function update_setting(obj)
    inst = guidata(obj);
    switch(obj)
        case {inst.threshold_edit}
            inst.threshold = str2num(get(obj, 'string'));
    endswitch

    inst.threshold = max(0, min(1, inst.threshold));

    guidata(obj, inst);
endfunction

function inst = render(inst)
    avg_x = sqrt(sumsq(inst.x) / length(inst.x));
    avg_pn = sqrt(sumsq(inst.pn) / inst.pn_len);
    pn_scale = avg_x / avg_pn;

    shifted_pn = pn_scale * circshift(inst.pn, inst.rxy_index - 1);
    shifted_pn_fft = circshift(fft(shifted_pn), inst.peak_offset - floor(inst.rxy_length / 2) - 1);
    shifted_pn_ifft = ifft(shifted_pn_fft);

    t = 0:1:(length(inst.pn) - 1)';
    set(inst.pn_s, 'xdata', t);
    set(inst.pn_s(1), 'ydata', inst.x');
    set(inst.pn_s(2), 'ydata', shifted_pn_ifft');
    ylim(inst.pn_axis, [-inf inf]);
    pn_lim = max(abs(ylim(inst.pn_axis)));
    ylim(inst.pn_axis, [-pn_lim pn_lim]);

    t = 0:1:(length(inst.pn) - 1)';
    set(inst.fft_s, 'xdata', t);
    set(inst.fft_s(1), 'ydata', fft(inst.x)');
    set(inst.fft_s(2), 'ydata', shifted_pn_fft');
    ylim(inst.fft_axis, [-inf inf]);
    fft_lim = max(abs(ylim(inst.fft_axis)));
    ylim(inst.fft_axis, [-fft_lim fft_lim]);

    t = 0:(length(inst.selected_rxy) - 1);
    set(inst.corr_s, 'xdata', t);
    set(inst.corr_s, 'ydata', abs(inst.selected_rxy)');

    set(inst.peak_value_label, 'string', num2str(abs(inst.frame_peak_corr)));
    set(inst.peak_freq_value_label, 'string', num2str(inst.frame_peak_offset - floor(inst.rxy_length / 2) - 1));
    set(inst.freq_peak_value_label, 'string', num2str(abs(inst.freq_peak_corr)));
    set(inst.freq_peak_sample_value_label, 'string', num2str(inst.freq_peak_offset));
    set(inst.sample_corr_value_label, 'string', num2str(abs(inst.selected_rxy(inst.rxy_index))));
    set(inst.phase_offset_edit, 'string', num2str(inst.rxy_index - 1));
    set(inst.correlation_offset_edit, 'string', num2str(inst.peak_offset - floor(inst.rxy_length / 2) - 1));
    set(inst.framenumber_edit, 'string', num2str(inst.framenumber));
    set(inst.threshold_edit, 'string', num2str(inst.threshold));
endfunction

function inst = load_frame(inst)
    load(['qdetector_out_' num2str(inst.framenumber) '.txt']);
    inst.selected_rxy = rxy(peak_offset,:);
    [_ rxy_index] = max(inst.selected_rxy);
    inst.peak_offset = peak_offset;
    inst.rxy_index = rxy_index;
    inst.frame_peak_offset = peak_offset;
    [inst.frame_peak_corr inst.freq_peak_offset] = max(rxy(peak_offset,:));
    inst.freq_peak_corr = inst.frame_peak_corr;
    inst.rxy = rxy;
    inst.rxy_length = size(inst.rxy, 1);
    inst.pn = pn;
    inst.x = x;
    inst.pn_len = pn_len;
    xlim(inst.pn_axis, [0 length(inst.pn)]);
    xlim(inst.fft_axis, [0 length(inst.pn)]);
    xlim(inst.corr_axis, [0 length(inst.selected_rxy)]);
endfunction

function update_frame(obj)
    inst = guidata(obj);
    switch(obj)
        case {inst.framenumber_edit}
            inst.framenumber = str2num(get(obj, 'string'));
        case {inst.framenumber_seek_prev_button}
            inst.framenumber = inst.framenumber - 1;
        case {inst.framenumber_seek_next_button}
            inst.framenumber = inst.framenumber + 1;
    endswitch

    inst.framenumber = max(0, min(length(inst.data_files) - 1, inst.framenumber));
    inst = load_frame(inst);
    inst = render(inst);
    guidata(obj, inst);
endfunction

function seek_frame(obj)
    inst = guidata(obj);
    range = inst.framenumber+1:length(inst.data_files);
    switch(obj)
        case {inst.threshold_seek_prev_button}
            range = inst.framenumber-1:-1:0;
    endswitch

    set(inst.threshold_edit, 'string', '... Seeking ...');

    for filenum = range
        set(inst.framenumber_edit, 'string', num2str(filenum));
        load(['qdetector_out_' num2str(filenum) '.txt'], '-text', 'peak');
        if peak >= inst.threshold
            inst.framenumber = filenum;
            break
        end
    end
    inst = load_frame(inst);
    inst = render(inst);
    guidata(obj, inst);
endfunction


inst.peak_offset = 0;
inst.rxy_index = 1;
inst.rxy_length = 0;
inst.threshold = 0.5;
inst.framenumber = 0;

inst.pn_axis = subplot('position', [0.05 0.68 0.65 0.26]);
t = (0:1:1)';
inst.pn_s = stem(t, [(0:1:1)', (0:1:1)'], 'visible', 'off');
set(inst.pn_s(2), 'linestyle', '--');
set(inst.pn_s(2), 'marker', '+');
set(gca, 'boxstyle', 'full');
box(gca, 'on');
set(get(gca,'title'), 'string', 'Received / PN');
zoom off;
zoom xon;

inst.fft_axis = subplot('position', [0.05 0.36 0.65 0.26]);
inst.fft_s = plot(t, [(0:1:1)', (0:1:1)'], 'visible', 'off');
set(inst.fft_s(2), 'linestyle', '--');
set(inst.fft_s(2), 'marker', '+');
set(get(gca, 'title'), 'string', 'Received / PN (FFT)');
zoom off;
zoom xon;

inst.corr_axis = subplot('position', [0.05 0.05 0.65 0.26]);
inst.corr_s = plot(t, (0:1:1), 'visible', 'off');
set(get(gca,'title'), 'string', 'Correlation');

inst.label_group = uibuttongroup('position', [0.77 0.63 0.18 0.31]);

inst.peak_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Peak Correlation (Frame)',
                            'horizontalalignment', 'left', 'position', [0.8 0.91 0.12 0.02]);
inst.peak_value_label = uicontrol('style', 'text', 'units', 'normalized', 'string', '0.85',
                            'horizontalalignment', 'center', 'position', [0.8 0.89 0.12 0.02]);

inst.peak_freq_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Freq Offset @ Peak',
                                 'horizontalalignment', 'left', 'position', [0.8 0.85 0.12 0.02]);
inst.peak_freq_value_label = uicontrol('style', 'text', 'units', 'normalized', 'string', '0.85',
                                       'horizontalalignment', 'center', 'position', [0.8 0.83 0.12 0.02]);

inst.freq_peak_freq_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Peak Correlation (Freq Offset)',
                                 'horizontalalignment', 'left', 'position', [0.8 0.79 0.12 0.02]);
inst.freq_peak_value_label = uicontrol('style', 'text', 'units', 'normalized', 'string', '0.85',
                                       'horizontalalignment', 'center', 'position', [0.8 0.77 0.12 0.02]);

inst.freq_peak_sample_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Sample Offset @ Peak',
                                 'horizontalalignment', 'left', 'position', [0.8 0.73 0.12 0.02]);
inst.freq_peak_sample_value_label = uicontrol('style', 'text', 'units', 'normalized', 'string', '0.85',
                                       'horizontalalignment', 'center', 'position', [0.8 0.71 0.12 0.02]);

inst.sample_corr_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Correlation',
                                 'horizontalalignment', 'left', 'position', [0.8 0.67 0.12 0.02]);
inst.sample_corr_value_label = uicontrol('style', 'text', 'units', 'normalized', 'string', '0.85',
                                       'horizontalalignment', 'center', 'position', [0.8 0.65 0.12 0.02]);

inst.control_group = uibuttongroup('position', [0.77 0.05 0.18 0.34]);

inst.phase_offset_edit_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Sample Offset',
                                         'horizontalalignment', 'left', 'position', [0.8 0.35 0.12 0.02]);
inst.phase_offset_edit = uicontrol('style', 'edit', 'units', 'normalized', 'string', '',
                                   'callback', @update_phase_offset, 'position', [0.8 0.33 0.12 0.02]);
inst.phase_offset_minus_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '-',
                                           'callback', @update_phase_offset, 'position', [0.8 0.31 0.06 0.02]);
inst.phase_offset_plus_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '+',
                                          'callback', @update_phase_offset, 'position', [0.86 0.31 0.06 0.02]);

inst.correlation_offset_edit_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Freq Offset',
                                               'horizontalalignment', 'left', 'position', [0.8 0.27 0.12 0.02]);
inst.correlation_offset_edit = uicontrol('style', 'edit', 'units', 'normalized', 'string', '',
                                         'callback', @update_freq_offset, 'position', [0.8 0.25 0.12 0.02]);
inst.correlation_offset_minus_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '-',
                                                 'callback', @update_freq_offset, 'position', [0.8 0.23 0.06 0.02]);
inst.correlation_offset_plus_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '+',
                                                'callback', @update_freq_offset, 'position', [0.86 0.23 0.06 0.02]);

inst.framenumber_edit_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Frame #',
                                        'horizontalalignment', 'left', 'position', [0.8 0.19 0.12 0.02]);
inst.framenumber_edit = uicontrol('style', 'edit', 'units', 'normalized', 'string', '',
                                  'callback', @update_frame, 'position', [0.8 0.17 0.12 0.02]);
inst.framenumber_seek_prev_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '<-',
                                              'callback', @update_frame, 'position', [0.8 0.15 0.06 0.02]);
inst.framenumber_seek_next_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '->',
                                              'callback', @update_frame, 'position', [0.86 0.15 0.06 0.02]);

inst.threshold_edit_label = uicontrol('style', 'text', 'units', 'normalized', 'string', 'Detect Threshold',
                                      'horizontalalignment', 'left', 'position', [0.8 0.11 0.12 0.02]);
inst.threshold_edit = uicontrol('style', 'edit', 'units', 'normalized', 'string', '',
                                'callback', @update_setting, 'position', [0.8 0.09 0.12 0.02]);
inst.threshold_seek_prev_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '<<-',
                                            'callback', @seek_frame, 'position', [0.8 0.07 0.06 0.02]);
inst.threshold_seek_next_button = uicontrol('style', 'pushbutton', 'units', 'normalized', 'string', '->>',
                                            'callback', @seek_frame, 'position', [0.86 0.07 0.06 0.02]);


inst.data_files = dir('qdetector_out_*.txt');

inst = load_frame(inst);
inst = render(inst);

xlim(inst.pn_axis, [0 length(inst.pn)]);
set(inst.pn_s, 'visible', 'on');
xlim(inst.fft_axis, [0 length(inst.pn)]);
set(inst.fft_s, 'visible', 'on');
axis(inst.corr_axis, [0 length(inst.selected_rxy) 0 1.5]);
set(inst.corr_s, 'visible', 'on');

guidata(gcf, inst);
