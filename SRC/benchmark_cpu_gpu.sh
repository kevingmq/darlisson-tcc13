#! /bin/bash
#===================================================================================
#
# FILE: benchmark_cpu_gpu.sh
#
# DESCRIPTION: Script para auxliar na execucao do benchmark do projeto de TCC
#  Usando o OpenCL e CPU
#
# AUTHOR: Darlisson Jesus <darlisson11@gmail.com>
# COMPANY: Universidade do Estado do Amazonas
# VERSION: 0.0.1
# CREATED: 04.08.2013
# REVISION:
#===================================================================================

USE_VENDOR="CPU" # 'ATI', 'CPU' or 'Nvidia'
USE_APP="Sobel" # 'FFT' or 'Sobel'
NV_520="GT_520"
NV_210="GT_210"
ATI_6540="HD_6540"
USE_CARD="${NV_520}"

CURRENT_DIR=$(pwd)
LOGS_DIR=${CURRENT_DIR}"/LOGS"
IMG_ORIGINAIS_DIR=${CURRENT_DIR}"/IMG_ORIGINAIS"
IMG_DB_DIR=${CURRENT_DIR}"/IMAGES_DB"
RESULTS_DIR=${CURRENT_DIR}"/RESULTS"
NVIDIA_DIR=${CURRENT_DIR}"/SRC/NVIDIA/Nvidia.TCC"
ATI_DIR=${CURRENT_DIR}"/SRC/ATI/ATI.TCC"
CPU_DIR=${CURRENT_DIR}"/SRC/CPU/CPU.TCC"
EXE_NAME=
# opcao para limpar pastas criadas
if [[ -n $1 && $1 == "--clear" ]]
then
    echo "Limpando diretorios .."
    rm -r ${LOGS_DIR}
    rm -r ${IMG_ORIGINAIS_DIR}
    exit 0
fi

case "$USE_VENDOR" in
"Nvidia")
        #LOG_APP=${NVIDIA_DIR}/"${USE_VENDOR}.${USE_APP}/global.log"
        LOG_APP=${NVIDIA_DIR}/"Debug/global.log"
        DEBUG_DIR=${NVIDIA_DIR}/"Debug"
    ;;
"ATI")
        #LOG_APP=${ATI_DIR}/"${USE_VENDOR}.${USE_APP}/global.log"
        LOG_APP=${ATI_DIR}/"Debug/global.log"
        DEBUG_DIR=${ATI_DIR}/"Debug"
    ;;
"CPU")
        LOG_APP=${CPU_DIR}/"Debug/global.log"
        DEBUG_DIR=${CPU_DIR}/"Debug"
    ;;
*)
    echo "Opcoes invalidas, verifique o USE_VENDOR"
    exit 0
    ;;
esac

SOBEL_EXE="${USE_VENDOR}.Sobel.exe"
FFT_EXE="${USE_VENDOR}.FFT.exe"

case "$USE_APP" in
"Sobel")
       APP=${SOBEL_EXE}
    ;;
"FFT")
       APP=${FT_EXE}
    ;;
*)
    echo "Opcoes invalidas, verifique o USE_APP"
    exit 0
    ;;
esac

echo -e "\n"
echo "+---------------------------------------------------------------------------+"
echo "| Comparacao de desempenho de rotinas para processamento digital de imagens |"
echo "+---------------------------------------------------------------------------+"
echo "| Resumo: AMBIENTE: ${USE_VENDOR} - MAQUINA: ${USE_CARD} - APLICACAO: ${USE_APP} |"
echo "+-------------------------------------------------------------+"

# criando pastas necessarias se elas nao existirem
echo "$(date +%k:%M:%S) INFO: Criando as pastas LOGS/ e ORIGINAIS/."
mkdir -p ${IMG_ORIGINAIS_DIR}
mkdir -p ${LOGS_DIR}

# fazendo uma copia das imagens originias para um dieretorio seguro
TOTAL_IMAGES=$( ls -l ${IMG_DB_DIR}/*.pgm | wc -l)
if [[ $(ls -l ${IMG_ORIGINAIS_DIR}/*.pgm | wc -l) -lt "${TOTAL_IMAGES}" ]]
    then
    echo "$(date +%k:%M:%S) INFO: Copiando imagens para a pasta ORIGINAIS/."
    cp ${IMG_DB_DIR}/*.pgm ${IMG_ORIGINAIS_DIR}/
fi

# adicionando cabecalho ao arquivo de log
echo "$(date +%k:%M:%S) INFO: Criando arquivo global.log ."
echo $'|resolucao|amostra|pixels|tempo|' > ${LOG_APP}

#  Navegando onde o .exe se encontra
echo "$(date +%k:%M:%S) INFO: Inicio da execucao."

cd ${DEBUG_DIR}

for img in $(ls -Sr ${IMG_ORIGINAIS_DIR}/*.pgm )
do
    ./${APP} "${img}"
done
echo "$(date +%k:%M:%S) INFO: Fim da execucao."

# Criando pasta para para salvar as imagens processadas
CURR_DATE=$(date +%d-%m-%Y-H%k-%M-%S)
RES_DIR=${RESULTS_DIR}"/${USE_VENDOR}/${USE_CARD}/${USE_APP}/${CURR_DATE}"

echo "$(date +%k:%M:%S) INFO: Criando pasta para os RESULTADOS."

mkdir -p ${RES_DIR}
# movendo images processadas para pasta especifica
echo "$(date +%k:%M:%S) INFO: Movendo imagens processadas para RESULTS/."
mv ${DEBUG_DIR}/*.pgm ${RES_DIR}/

# salvando arquivo de log
cp ${LOG_APP} ${LOGS_DIR}"/${USE_VENDOR}.${USE_CARD}.${USE_APP}.${CURR_DATE}.dat"


echo "+--------------------------------------------------------------------------+"
echo "| Imagens processadas : $(ls -l ${RES_DIR}/*.pgm | wc -l )                 |"
echo "| log: ${LOGS_DIR}/${USE_VENDOR}.${USE_CARD}.${USE_APP}.${CURR_DATE}.log   |"
echo "+----------------------------------------------------------------------------------+"

echo -e "\n"

exit 0