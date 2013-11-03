
# sets the path of the raw data
kBase.path <- "C:\\Users\\Darlisson\\Google Drive\\results\\"
kGraph.path <- paste(kBase.path, "graphs", sep="\\")

# sets the splited files name formation
kNvidia.520 <- "gt_520"
kNvidia.210 <- "gt_210"
kAti.6450 <- "hd_6450"
kPlatform.CPU <- "CPU"
kPlatform.Nvidia <- "Nvidia"
kPlatform.Ati <- "ATI"
kApp.Sobel <- "Sobel"
kApp.FFT <- "FFT"
kApp.FFT.graph <- "Passa-baixa"
kLog.ext <- "dat"
# columns num of data table object
kResolucao <- 2
kTempoKernel.cpu <- 5
kTempoTotal.cpu <- 6
kTempoKernel.gpu <- 5
kCopyHost2Dev <- 6
kCopyDev2Host <- 7
kTempoTotal.gpu <- 8

# Metric to plot Average or Standard Deviation
kSd <- 0
kMean <- 1
kVar <- 2

# Graph plot type, linear or bar
kLinearPlot <- 1
kBarPlot <- 0

# Platforms string names
kApp.types <- c(kApp.Sobel, kApp.FFT)
kApp.graph.title <- c(kApp.Sobel, kApp.FFT.graph)

platforms <- c("CPU","GPU")

# Applicaton code
kSobel <- 1
kFFT <- 2

# files name formation
files.names <- c(
                paste(kPlatform.CPU, kNvidia.520, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.CPU, kNvidia.210, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.CPU, kAti.6450, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.CPU, kNvidia.520, kApp.FFT, kLog.ext, sep="."),
                paste(kPlatform.CPU, kNvidia.210, kApp.FFT, kLog.ext, sep="."),
                paste(kPlatform.CPU, kAti.6450, kApp.FFT, kLog.ext, sep="."),
                paste(kPlatform.Nvidia, kNvidia.520, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.Nvidia, kNvidia.210, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.Ati, kAti.6450, kApp.Sobel, kLog.ext, sep="."),
                paste(kPlatform.Nvidia, kNvidia.520, kApp.FFT, kLog.ext, sep="."),
                paste(kPlatform.Nvidia, kNvidia.210, kApp.FFT, kLog.ext, sep="."),
                paste(kPlatform.Ati, kAti.6450, kApp.FFT, kLog.ext, sep=".")
)

# graphs definitions
graph.xlabel <- "Tamanho das amostras em Megapixels"
graph.pch <- c(15,16,17,18)
graph.font.label <- 1
graph.lwd <- 1
graph.plot.type <- "o"
graph.line.types <- c("solid", "dotdash", "dashed", "longdash")
graph.colors <- c("#E41A1C", "#377EB8","#4DAF4A","#C70EE7" )
graph.axis.at <- c(256, 512, 1024, 2048, 4096, 8192)
#graph.axis.labels <- c("256x256", "512x512", "1024x1024", "2048x2048", "4096x4096", "8192x8192")
graph.axis.labels <- c("0.06", "0.26", "1.00", "4.19", "16.77", "67.10")
graph.legend.title<- "Modelo de Processador"
graph.legend.values<- c("NVIDIA GT 520", "NVIDIA GT 210", "ATI HD 6450", "Intel Core 2 Duo")

# the title of graphs to plot
graph.titles <- list(
c("Tempo Médio de execução do filtro", "Tempo em segundos na GPU"),
c("Tempo Médio de execução total", "Tempo em segundos na GPU"))

graph.pdf.names <- list(
c("Tempo_Medio_de_execucao_do_filtro"),
c("Tempo_Medio_de_execucao_total"))

# load files data as table object
# data structure  e.g |123|321|456|
cpu.520.sobel.data <- read.table(paste(kBase.path, files.names[1], sep="\\"), sep="|", h=T)
cpu.210.sobel.data <- read.table(paste(kBase.path, files.names[2], sep="\\"), sep="|", h=T)
cpu.6450.sobel.data <- read.table(paste(kBase.path, files.names[3], sep="\\"), sep="|", h=T)
cpu.520.fft.data <- read.table(paste(kBase.path, files.names[4], sep="\\"), sep="|", h=T)
cpu.210.fft.data <- read.table(paste(kBase.path, files.names[5], sep="\\"), sep="|", h=T)
cpu.6450.fft.data <- read.table(paste(kBase.path, files.names[6], sep="\\"), sep="|", h=T)
nvidia.520.sobel.data <- read.table(paste(kBase.path, files.names[7], sep="\\"), sep="|", h=T)
nvidia.210.sobel.data <- read.table(paste(kBase.path, files.names[8], sep="\\"), sep="|", h=T)
ati.6450.sobel.data <- read.table(paste(kBase.path, files.names[9], sep="\\"), sep="|", h=T)
nvidia.520.fft.data <- read.table(paste(kBase.path, files.names[10], sep="\\"), sep="|", h=T)
nvidia.210.fft.data <- read.table(paste(kBase.path, files.names[11], sep="\\"), sep="|", h=T)
ati.6450.fft.data <- read.table(paste(kBase.path, files.names[12], sep="\\"), sep="|", h=T)

for (app.type in 1:2) {

graph.legend.title <- graph.legend.title
graph.legend.values <- graph.legend.values

if (app.type == kSobel){
    graph.data.cpu <- list(cpu.520.sobel.data,
                   cpu.210.sobel.data,
                   cpu.6450.sobel.data)
    } else {
      graph.data.cpu <- list(cpu.520.fft.data,
                   cpu.210.fft.data,
                   cpu.6450.fft.data)
    }

if (app.type == kSobel){
    graph.data.gpu <- list(nvidia.520.sobel.data,
                   nvidia.210.sobel.data,
                   ati.6450.sobel.data)
    } else {
      graph.data.gpu <- list(nvidia.520.fft.data,
                   nvidia.210.fft.data,
                   ati.6450.fft.data)
}

for (graph.choice in 1:2) {

if (graph.choice == 1){
  column.data.cpu <- kTempoKernel.cpu
  column.data.gpu <- kTempoKernel.gpu
} else {
  column.data.cpu <- kTempoTotal.cpu
  column.data.gpu <- kTempoTotal.gpu
  }


graph.ylabel <- graph.titles[[graph.choice]][2]
graph.title <- paste(kApp.graph.title[app.type], graph.titles[[graph.choice]][1], sep=" - ")
graph.filename <- paste(kApp.types[app.type], graph.pdf.names[graph.choice], sep="_")

graph.xlim <- c(128, 8196)

pdf.file = paste(paste(kGraph.path, graph.filename, sep="\\"), ".pdf", sep="")

pdf(pdf.file, width=9, height=7) # in inch
# Allocate a vector with 6 spaces
data.resolucao <- rep(NA, 6)
data.520.gpu <- rep(NA, 6)
data.210.gpu <- rep(NA, 6)
data.6450.gpu <- rep(NA, 6)
data.520.cpu <- rep(NA, 6)
data.210.cpu <- rep(NA, 6)
data.6450.cpu <- rep(NA, 6)

    #workaround to convert column data from miliseconds to seconds
    divisor <- 1

    if (column.data.cpu %in% c(kTempoKernel.cpu, kTempoKernel.gpu)){
      divisor <- 1000
    }

    for (n in 1:6) {
        data.resolucao[n] = mean(graph.data.gpu[[3]][[kResolucao]][((n-1)*8 + 1):(n*8)])
        data.520.gpu[n] = mean(graph.data.gpu[[1]][[column.data.gpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor
        data.210.gpu[n] = mean(graph.data.gpu[[2]][[column.data.gpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor
        data.6450.gpu[n] = mean(graph.data.gpu[[3]][[column.data.gpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor

        data.520.cpu[n] = mean(graph.data.cpu[[1]][[column.data.cpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor
        data.210.cpu[n] = mean(graph.data.cpu[[2]][[column.data.cpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor
        data.6450.cpu[n] = mean(graph.data.cpu[[3]][[column.data.cpu]][((n-1)*8 + 1):(n*8)], na.rm=TRUE)/divisor

      }

    data.cpu <- data.520.cpu + data.210.cpu + data.6450.cpu
    data.cpu <- data.cpu/3

max.ylim <- max(data.210.gpu, na.rm=TRUE)

if (max(data.6450.gpu) > max.ylim) {
    max.ylim <- max(data.6450.gpu)
}

if (max(data.520.gpu, na.rm=TRUE) > max.ylim) {
  max.ylim <- max(data.520.gpu, na.rm=TRUE)
}

graph.ylim <- c(0, max.ylim*(1.3))

par(mar=c(5,4,4,5)+.1)

    plot(x        = data.resolucao,
         y        = data.210.gpu,
         ylim     = graph.ylim,
         xlim     = graph.xlim,
         xaxt     = "n",
         type     = graph.plot.type,
         pch      = graph.pch[2],
         col      = graph.colors[2],
         lwd      = graph.lwd,
         lty      = graph.line.types[2],
         xlab     = graph.xlabel,
         ylab     = graph.ylabel,
         font.lab = graph.font.label,
         cex.axis = 1)

    points(x    = data.resolucao,
           y    = data.520.gpu,
           type = graph.plot.type,
           pch  = graph.pch[1],
           col  = graph.colors[1],
           lwd  = graph.lwd,
           lty  = graph.line.types[1])

    points(x = data.resolucao,
       y    = data.6450.gpu,
       type = graph.plot.type,
       pch  = graph.pch[3],
       col  = graph.colors[3],
       lwd  = graph.lwd,
       lty  = graph.line.types[3])

    par(new=TRUE)

    max.ylim <- max(data.cpu, na.rm=TRUE)
    max.ylim <- max.ylim*(1.3)
    graph.ylim <- c(0, max.ylim)

  if(app.type == kFFT) {
    plot(data.resolucao, data.cpu,,
         type = graph.plot.type,
         #ylim = graph.ylim,
         xlim = graph.xlim,
         col  = graph.colors[4],
         pch  = graph.pch[4],
         lwd  = graph.lwd,
         lty  = graph.line.types[4],
         xaxt ="n",
         yaxt ="n",
         xlab ="",
         ylab ="",
         cex.axis = 1)
    } else {
          plot(data.resolucao, data.cpu,,
         type = graph.plot.type,
         ylim = graph.ylim,
         xlim = graph.xlim,
         col  = graph.colors[4],
         pch  = graph.pch[4],
         lwd  = graph.lwd,
         lty  = graph.line.types[4],
         xaxt ="n",
         yaxt ="n",
         xlab ="",
         ylab ="",
         cex.axis = 1)
    }

    abline(v=c(256, 512, 1024, 2048, 4096, 8192), col="gray", lty="dotted")
    abline(h=(seq(0, max.ylim,  max.ylim/10)), col="gray", lty="dotted")

    axis(side     = 1,
         at       = graph.axis.at,
    #    labels   = expression(2^16, 2^18, 2^20, 2^22, 2^24, 2^26),
         labels   = graph.axis.labels,
         cex.axis = 1)

    axis(4)
    mtext("Tempo em segundos na CPU", side=4, line=3)

title(graph.title)

legend(x = "topleft",
       title = graph.legend.title, graph.legend.values,
       col   = graph.colors,
       lty   = graph.line.types,
       lwd   = graph.lwd,
       pch   = graph.pch,
       xpd   = T,
       bg = "white")

dev.off()

rm(data.520.gpu)
rm(data.210.gpu)
rm(data.6450.gpu)
rm(data.520.cpu)
rm(data.210.cpu)
rm(data.6450.cpu)
  }
}