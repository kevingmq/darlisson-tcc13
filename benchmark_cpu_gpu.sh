#! /bin/bash
#===================================================================================
#
# FILE: benchmark_cpu_gpu.sh
#
# DESCRIPTION: Script para auxliar na execucao do benchmark do projeto de TCC
#  Usando o OpenCL e CPU
#
# AUTHOR: Darlisson Jesus <darlisson11@gmail.com> - setembro de 2013
# COMPANY: Universidade do Estado do Amazonas
# VERSION: 0.1

#===================================================================================
VERSION="0.1"
NV_520="gt_520"
NV_210="gt_210"
ATI_6450="hd_6450"

CURRENT_DIR=$(pwd)
LOGS_DIR=${CURRENT_DIR}"/LOGS"
IMG_ORIGINAIS_DIR=${CURRENT_DIR}"/IMG_ORIGINAIS"
IMG_DB_DIR=${CURRENT_DIR}"/IMAGES_DB"
RESULTS_DIR=${CURRENT_DIR}"/RESULTS"
NVIDIA_DIR=${CURRENT_DIR}"/SRC/NVIDIA/Nvidia.TCC"
ATI_DIR=${CURRENT_DIR}"/SRC/ATI/ATI.TCC"
CPU_DIR=${CURRENT_DIR}"/SRC/CPU/CPU.TCC"
KERNEL_DIR=${CURRENT_DIR}"/SRC/utils/kernels"
SCRIPT_NAME=$(basename $0)

SCRIPT_ARGS="$@"

MENSAGEM_USO="
Uso : $(basename "$0") [OPÇÕES] ... [-p -a -c ]

OPÇÕES:
[obrigatorios]
    -p, --platform       Parametro obrigatorio e diz respeito a plataforma sobre teste
    -a, --app           Parametro obrigatorio e diz respeito a aplicacao para o teste
    -c, --card          Parametro obrigatorio e diz respeito a placa grafica sobre teste

[opcionais]
    -l, --list          Mostra os valores dos parametros permitidos.
    -h, --help          Mostra essa tela de ajuda e sai
    -V, --version       Mostra a versao atual do programa e sai
    -cl, --clean        Limpa os diretorios de um processamento
"

MSG_LIST="
Valores dos argumentos suportados por essa versao:
-p, --platform  ['Nvidia' | 'ATI' | 'CPU' ]
-a, --app       [ 'Sobel' | 'FFT']
-c, --card      [ 'gt_520' | 'gt_210' | 'hd_6450']
"


#Tratamento das opcoes da linha de comando
while test -n "$1"
do
    case "$1" in
        -h | --help)
            echo "${MENSAGEM_USO}"
            exit 0
            ;;

        -V | --version)
            echo -n $(basename "$0")
            echo " versao ${VERSION}"
            exit 0
            ;;

        -p | --platform)
            shift
            USE_VENDOR="$1"
            if test -z "${USE_VENDOR}"
            then
                echo "$(date +%H:%M:%S) INFO: '$1' opção desconhecida para  -p, --platform"
                echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                exit 1
            else
                case "$USE_VENDOR" in
                        "Nvidia")

                                LOG_APP=${NVIDIA_DIR}/"x64/Debug/global.log"
                                DEBUG_DIR=${NVIDIA_DIR}/"x64/Debug"
                                if [[ "$(ls -l ${DEBUG_DIR}/*.cl  2>/dev/null| wc -l)"  < 2 ]]
                                    then
                                    echo "$(date +%H:%M:%S) INFO: Copiando *.cl kernel para Debug/."
                                    cp ${KERNEL_DIR}/*.cl ${DEBUG_DIR}/
                                fi
                            ;;
                        "ATI")

                                LOG_APP=${ATI_DIR}/"x64/Debug/global.log"
                                DEBUG_DIR=${ATI_DIR}/"x64/Debug"
                                if [[ "$(ls -l ${DEBUG_DIR}/*.cl  2>/dev/null| wc -l)"  < 2 ]]
                                    then
                                    echo "$(date +%H:%M:%S) INFO: Copiando *.cl kernel para Debug/."
                                    cp ${KERNEL_DIR}/*.cl ${DEBUG_DIR}/
                                fi
                            ;;
                        "CPU")
                                LOG_APP=${CPU_DIR}/"x64/Debug/global.log"
                                DEBUG_DIR=${CPU_DIR}/"x64/Debug"
                            ;;
                        *)
                             echo "$(date +%H:%M:%S) INFO: '${USE_VENDOR}' opção desconhecida para  -p, --platform"
                             echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                             exit 1
                            ;;
                        esac
            fi
            ;;

        -c | --card)
            shift
            USE_CARD="$1"
            if test -z "${USE_CARD}"; then
                echo "$(date +%H:%M:%S) INFO: '$1' opção desconhecida para  -c, --card"
                echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                exit 1
            else
                case "$USE_CARD" in
                    "gt_520")
                           USE_CARD=${NV_520}
                        ;;
                    "gt_210")
                           USE_CARD=${NV_210}
                        ;;
                    "hd_6450")
                           USE_CARD=${ATI_6450}
                        ;;
                    *)
                        echo "$(date +%H:%M:%S) INFO: '${USE_APP}' opção desconhecida para  -c, --card"
                        echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                        exit 1
                        ;;
                esac
            fi
            ;;

        -a | --app)
            shift
            USE_APP="$1"
            if test -z "${USE_APP}"; then
                echo "$(date +%H:%M:%S) INFO: '$1' opção desconhecida para  -a, --app"
                echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                exit 1
            else
                case "$USE_APP" in
                    "Sobel")
                           APP_CHOICE=${USE_APP}
                        ;;
                    "FFT")
                           APP_CHOICE=${USE_APP}
                        ;;
                    *)
                        echo "$(date +%H:%M:%S) INFO: '${USE_APP}' opção desconhecida para  -a, --app"
                        echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -l para mais informações"
                        exit 1
                        ;;
                esac
            fi
            ;;

        -l | --list)
            echo "${MSG_LIST}"
            exit 0
            ;;

        -cl | --clean)
            rm -r ${LOGS_DIR} 2>/dev/null
            echo "$(date +%H:%M:%S) INFO: Removido o diretorio LOGS/"
            rm -r ${IMG_ORIGINAIS_DIR} 2>/dev/null
            echo "$(date +%H:%M:%S) INFO: Removido o diretorio IMG_ORIGINAIS/"
            read -p "Deseja remover o diretorio com os resultados tambem? (y/n): " choice
            case "$choice" in
                y|Y )
                    #rm -r ${RESULTS_DIR} 2>/dev/null
                    echo "$(date +%H:%M:%S) INFO: Removido o diretorio RESULTS/"
                ;;
                n|N ) echo "$(date +%H:%M:%S) INFO: Mantido o diretorio RESULTS/"
                ;;
                * ) echo "opcao invalida";;
            esac
            exit 0
        ;;

        *)
            echo "$(date +%H:%M:%S) INFO: ${SCRIPT_NAME} : '$1' opção desconhecida"
            echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -h para mais informações"
            exit 1
    esac
    shift
done

if [[ "$(echo "${SCRIPT_ARGS}" | awk '{n=split($0, array, " ")} END{print n }')"  = 6 ]]
then
    if [[ -z "$USE_VENDOR" ]]; then
        echo "$(date +%H:%M:%S) INFO: ${SCRIPT_NAME} : '-p' argumento obrigatório"
        echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -h para mais informações"
        exit 0
        elif [[ -z "$USE_APP" ]]; then
            echo "$(date +%H:%M:%S) INFO: ${SCRIPT_NAME} : '-a' argumento obrigatório"
            echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -h para mais informações"
            exit 0
            elif [[ -z "$USE_CARD" ]]; then
                echo "$(date +%H:%M:%S) INFO: ${SCRIPT_NAME} : '-c' argumento obrigatório"
                echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -h para mais informações"
                exit 0
    fi
else
    echo "$(date +%H:%M:%S) INFO: ${SCRIPT_NAME} : numero de argumentos insuficientes"
    echo "$(date +%H:%M:%S) INFO: Tente ${SCRIPT_NAME} : -h para mais informações"
    exit 0
fi

APP_CHOICE="${USE_VENDOR}.${USE_APP}.exe"

ESCAPE="               "
echo -e "\n"
echo "+---------------------------------------------------------------------------+"
echo "| Comparacao de desempenho de rotinas para processamento digital de imagens |"
echo "+---------------------------------------------------------------------------+"
echo "| Resumo: AMBIENTE: ${USE_VENDOR} - CARD: ${USE_CARD} - APLICACAO: ${USE_APP} ${ESCAPE}|"
echo "+---------------------------------------------------------------------------+"


# criando pastas necessarias se elas nao existirem
echo "$(date +%H:%M:%S) INFO: Criando os diretorios LOGS/ e IMG_ORIGINAIS/."
mkdir -p ${IMG_ORIGINAIS_DIR}
mkdir -p ${LOGS_DIR}
mkdir -p ${RESULTS_DIR}

# fazendo uma copia das imagens originias para um dieretorio seguro
TOTAL_IMAGES=$( ls -l ${IMG_DB_DIR}/*.pgm 2>/dev/null | wc -l)
if [[ $(ls -l ${IMG_ORIGINAIS_DIR}/*.pgm 2>/dev/null | wc -l) -lt "${TOTAL_IMAGES}" ]]
    then
    echo "$(date +%H:%M:%S) INFO: Copiando imagens para a diretorios ORIGINAIS/."
    cp ${IMG_DB_DIR}/*.pgm ${IMG_ORIGINAIS_DIR}/
fi

# adicionando cabecalho ao arquivo de log
echo "$(date +%H:%M:%S) INFO: Criando arquivo global.log ."
if [[ "$USE_VENDOR" != "CPU" ]]; then
    echo $'|resolucao|amostra|pixels|tempoKernel[ms]|CopyHostToDev[MB/s]|CopyDevToHost[MB/s]|Tempo Total[s]|' > ${LOG_APP}
else
    echo $'|resolucao|amostra|pixels|tempoKernel[ms]|Tempo Total[s]|' > ${LOG_APP}
fi

#  Navegando onde o .exe se encontra
echo "$(date +%H:%M:%S) INFO: Inicio da execucao."

cd ${DEBUG_DIR}

 for img in $(ls -1v ${IMG_ORIGINAIS_DIR}/*.pgm )
 do
     ./${APP_CHOICE} "${img}"
 done

# Para teste
# for ((i=1; i <= 8 ; i++))
# do
#     ./${APP_CHOICE} ${IMG_ORIGINAIS_DIR}/256.$i.pgm
# done

echo "$(date +%H:%M:%S) INFO: Fim da execucao."
# Criando pasta para para salvar as imagens processadas
CURR_DATE=$(date +%d-%m-%Y-H%H-%M-%S)
RES_DIR=${RESULTS_DIR}"/${USE_VENDOR}/${USE_CARD}/${USE_APP}/${CURR_DATE}"

echo "$(date +%H:%M:%S) INFO: Criando diretorios para os RESULTADOS"

mkdir -p ${RES_DIR}
# movendo images processadas para pasta especifica
if [[ $(ls -l ${DEBUG_DIR}/*.pgm 2>/dev/null| wc -l) = 0 ]]
then
    echo "$(date +%H:%M:%S) INFO: Nenhuma imagem foi processada"
else
    echo "$(date +%H:%M:%S) INFO: Movendo imagens processadas para o diretorio RESULTS/"
    mv ${DEBUG_DIR}/*.pgm ${RES_DIR}/
fi

# salvando arquivo de log
cp ${LOG_APP} ${LOGS_DIR}"/${USE_VENDOR}.${USE_CARD}.${USE_APP}.dat"

echo "+--------------------------------------------------------------------------+"
echo "| Imagens processadas : $(ls -l ${RES_DIR}/*.pgm | wc -l )                 |"
echo "| log: ${LOGS_DIR}/${USE_VENDOR}.${USE_CARD}.${USE_APP}.dat   |"
echo "+----------------------------------------------------------------------------------+"

echo -e "\n"
exit 0