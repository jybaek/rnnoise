#!/bin/sh

ret=0
srcdir=`dirname $0`

SPEECH_NOISE_FILE=$1

if [ -z "${SPEECH_NOISE_FILE}" ] || [ ! -f ${SPEECH_NOISE_FILE} ];then
    echo "${SPEECH_NOISE_FILE}: ERROR: No such file"
    exit $ret
fi

DIR=$(dirname ${SPEECH_NOISE_FILE})
FILE=$(basename ${SPEECH_NOISE_FILE} | cut -d'.' -f1)
EXT=$(basename ${SPEECH_NOISE_FILE} | cut -d'.' -f2)

if [ ${EXT} != "pcm" ];then
    echo "${EXT}: ERROR: mismatch extension. 'pcm' only."
    exit $ret
fi

DENOISED_SPEECH_PCM_FILE="${DIR}/denoised_${FILE}.${EXT}"
DENOISED_SPEECH_WAV_FILE="${DIR}/denoised_${FILE}.wav"

${srcdir}/examples/rnnoise_demo ${SPEECH_NOISE_FILE} ${DENOISED_SPEECH_PCM_FILE} || ret=1
if [ $ret != 0 ];then echo "ERROR: ${ret}";exit $ret;fi

${srcdir}/wavutils/bin/pcm2wav 1 16000 16 ${DENOISED_SPEECH_PCM_FILE} ${DENOISED_SPEECH_WAV_FILE} || ret=1
if [ $ret != 0 ];then echo "ERROR: ${ret}";exit $ret;fi

echo denoised speech : ${DENOISED_SPEECH_WAV_FILE} || ret=1

exit $ret
