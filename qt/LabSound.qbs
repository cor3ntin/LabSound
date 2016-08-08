import qbs

StaticLibrary {
    name:"LabSound"

    Depends { name: "cpp" }
    Depends { name: "Qt.multimedia" }

    cpp.cxxLanguageVersion : "c++11"
    cpp.minimumOsxVersion : "10.7"

    Group {
        name: "core"
        files : [ "*.cpp" ]
        prefix : "../src/core/"
    }
    Group {
        name: "internal"
        files : [ "*.cpp" ]
        prefix : "../src/internal/src/"
        excludeFiles: [
            "AudioFileReader.cpp"
        ]
    }
    /*Group {
        name: "test"
        files : [ "*.cpp" ]
        prefix : "../src/internal/src/win/"
    }*/
    Group {
        name: "extended"
        files : [ "*.cpp" ]
        prefix : "../src/extended/"

        excludeFiles: [
            "RecorderNode.cpp"
        ]
    }

    Group {
        name: "kissfft"
        files : [ "*.cpp" ]
        prefix : "../third_party/kissfft/src/"
    }

    Group {
        name: "ooura"
        files : [ "*.cpp" ]
        prefix : "../third_party/ooura/src/"
    }

    Group {
        name: "Qt"
        files  : [ "*.cpp", "*.h"]
        prefix : "./"
    }


    Group {
        name: "OGG"

        files : [
               "*.c"
        ]
        prefix : "third_party/libogg-1.3.2/src/"
    }

    /*Group {
         name: "Vorbis"
         files : [
             "mdct.c", "smallft.c", "block.c", "envelope.c", "window.c", "lpc.c", "analysis.c",
             "synthesis.c", "psy.c", "info.c", "floor1.c", "floor0.c", "res0.c","mapping0.c",
             "registry.c", "codebook.c", "sharedbook.c", "lookup.c", "bitrate.c", "envelope.h",
             "lpc.h", "lsp.c", "codebook.h", "misc.h", "psy.h", "masking.h", "os.h", "mdct.h",
             "smallft.h", "highlevel.h", "registry.h", "scales.h", "window.h",
             "lookup.h", "lookup_data.h", "codec_internal.h", "backends.h", "bitrate.h",
             "vorbisfile.c"
         ]
         prefix: "third_party/libvorbis-1.3.5/lib/"
    }*/

    Group {
        name: "Opus"
        files : [
            "silk/control.h",
            "silk/debug.h",
            "silk/define.h",
            "silk/errors.h",
            "silk/MacroCount.h",
            "silk/MacroDebug.h",
            "silk/macros.h",
            "silk/main.h",
            "silk/pitch_est_defines.h",
            "silk/PLC.h",
            "silk/resampler_private.h",
            "silk/resampler_rom.h",
            "silk/resampler_structs.h",
            "silk/API.h",
            "silk/SigProc_FIX.h",
            "silk/structs.h",
            "silk/tables.h",
            "silk/tuning_parameters.h",
            "silk/typedef.h",

            "silk/A2NLSF.c",
            "silk/ana_filt_bank_1.c",
            "silk/biquad_alt.c",
            "silk/bwexpander.c",
            "silk/bwexpander_32.c",
            "silk/check_control_input.c",
            "silk/CNG.c",
            "silk/code_signs.c",
            "silk/control_audio_bandwidth.c",
            "silk/control_codec.c",
            "silk/control_SNR.c",
            "silk/debug.c",
            "silk/decoder_set_fs.c",
            "silk/decode_core.c",
            "silk/decode_frame.c",
            "silk/decode_indices.c",
            "silk/decode_parameters.c",
            "silk/decode_pitch.c",
            "silk/decode_pulses.c",
            "silk/dec_API.c",
            "silk/encode_indices.c",
            "silk/encode_pulses.c",
            "silk/enc_API.c",
            "silk/gain_quant.c",
            "silk/HP_variable_cutoff.c",
            "silk/init_decoder.c",
            "silk/init_encoder.c",
            "silk/inner_prod_aligned.c",
            "silk/interpolate.c",
            "silk/lin2log.c",
            "silk/log2lin.c",
            "silk/LPC_analysis_filter.c",
            "silk/LPC_inv_pred_gain.c",
            "silk/LP_variable_cutoff.c",
            "silk/NLSF2A.c",
            "silk/NLSF_decode.c",
            "silk/NLSF_del_dec_quant.c",
            "silk/NLSF_encode.c",
            "silk/NLSF_stabilize.c",
            "silk/NLSF_unpack.c",
            "silk/NLSF_VQ.c",
            "silk/NLSF_VQ_weights_laroia.c",
            "silk/NSQ.c",
            "silk/NSQ_del_dec.c",
            "silk/pitch_est_tables.c",
            "silk/PLC.c",
            "silk/process_NLSFs.c",
            "silk/quant_LTP_gains.c",
            "silk/resampler.c",
            "silk/resampler_down2.c",
            "silk/resampler_down2_3.c",
            "silk/resampler_private_AR2.c",
            "silk/resampler_private_down_FIR.c",
            "silk/resampler_private_IIR_FIR.c",
            "silk/resampler_private_up2_HQ.c",
            "silk/resampler_rom.c",
            "silk/shell_coder.c",
            "silk/sigm_Q15.c",
            "silk/sort.c",
            "silk/stereo_decode_pred.c",
            "silk/stereo_encode_pred.c",
            "silk/stereo_find_predictor.c",
            "silk/stereo_LR_to_MS.c",
            "silk/stereo_MS_to_LR.c",
            "silk/stereo_quant_pred.c",
            "silk/sum_sqr_shift.c",
            "silk/tables_gain.c",
            "silk/tables_LTP.c",
            "silk/tables_NLSF_CB_NB_MB.c",
            "silk/tables_NLSF_CB_WB.c",
            "silk/tables_other.c",
            "silk/tables_pitch_lag.c",
            "silk/tables_pulses_per_block.c",
            "silk/table_LSF_cos.c",
            "silk/VAD.c",
            "silk/VQ_WMat_EC.c",

            "silk/float/main_FLP.h",
            "silk/float/SigProc_FLP.h",
            "silk/float/structs_FLP.h",

            "silk/float/apply_sine_window_FLP.c",
            "silk/float/autocorrelation_FLP.c",
            "silk/float/burg_modified_FLP.c",
            "silk/float/bwexpander_FLP.c",
            "silk/float/corrMatrix_FLP.c",
            "silk/float/encode_frame_FLP.c",
            "silk/float/energy_FLP.c",
            "silk/float/find_LPC_FLP.c",
            "silk/float/find_LTP_FLP.c",
            "silk/float/find_pitch_lags_FLP.c",
            "silk/float/find_pred_coefs_FLP.c",
            "silk/float/inner_product_FLP.c",
            "silk/float/k2a_FLP.c",
            "silk/float/levinsondurbin_FLP.c",
            "silk/float/LPC_analysis_filter_FLP.c",
            "silk/float/LPC_inv_pred_gain_FLP.c",
            "silk/float/LTP_analysis_filter_FLP.c",
            "silk/float/LTP_scale_ctrl_FLP.c",
            "silk/float/noise_shape_analysis_FLP.c",
            "silk/float/pitch_analysis_core_FLP.c",
            "silk/float/prefilter_FLP.c",
            "silk/float/process_gains_FLP.c",
            "silk/float/regularize_correlations_FLP.c",
            "silk/float/residual_energy_FLP.c",
            "silk/float/scale_copy_vector_FLP.c",
            "silk/float/scale_vector_FLP.c",
            "silk/float/schur_FLP.c",
            "silk/float/solve_LS_FLP.c",
            "silk/float/sort_FLP.c",
            "silk/float/warped_autocorrelation_FLP.c",
            "silk/float/wrappers_FLP.c",

            "libopus/src/opus.c",
            "libopus/src/opus_decoder.c",
            "libopus/src/opus_encoder.c",
            "libopus/src/opus_multistream.c",
            "libopus/src/opus_multistream_decoder.c",
            "libopus/src/opus_multistream_encoder.c",
            "libopus/src/repacketizer.c",

            "libopus/src/analysis.c",
            "libopus/src/mlp.c",
            "libopus/src/mlp_data.c",

            "opusfile/src/http.c",
            "opusfile/src/info.c",
            "opusfile/src/internal.c",
            "opusfile/src/opusfile.c",
            "opusfile/src/stream.c",

            "celt/bands.c",
            "celt/celt.c",
            "celt/celt_decoder.c",
            "celt/celt_encoder.c",
            "celt/celt_lpc.c",
            "celt/cwrs.c",
            "celt/entcode.c",
            "celt/entdec.c",
            "celt/entenc.c",
            "celt/kiss_fft.c",
            "celt/laplace.c",
            "celt/mathops.c",
            "celt/mdct.c",
            "celt/modes.c",
            "celt/opus_custom_demo.c",
            "celt/pitch.c",
            "celt/quant_bands.c",
            "celt/rate.c",
            "celt/vq.c",
        ]
        prefix: "third_party/opus/"
    }

    Depends { name: "Qt.multimedia" }

    cpp.includePaths : ['../include', '../src', '../third_party',
        'third_party/libogg-1.3.2/include',
        'third_party/opus/libopus/include',
        'third_party/opus/opusfile/include',
        'third_party/opus/silk/float/',
        'third_party/opus/silk/fixed/',
        'third_party/opus/silk/',
        'third_party/opus/celt',
        'third_party/opus/opusfile/src/include/'
    ]
    cpp.defines : [ 'WTF_USE_WEBAUDIO_KISSFFT' , 'HAVE_CONFIG_H']

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.multimedia" }
        cpp.includePaths : ['../include', '../src', '../third_party']

        Properties {
            condition: qbs.targetOS.contains("osx")
            cpp.frameworks: ["Accelerate"]
        }
    }
}
