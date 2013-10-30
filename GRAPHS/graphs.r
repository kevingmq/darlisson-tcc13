
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
kLog.ext <- "dat"
# columns num of data table object
kResolucao <- 2
kTempoKernel.cpu <- 5
kTempoTotal.cpu <- 6
kTempoKernel.gpu <- 5
kTempoTotal.gpu <- 8
kCopyHost2Dev <- 6
kCopyDev2Host <- 7

# Metric to plot Average or Standard Deviation
kSd <- 0
kMean <- 1
kVar <- 2

# Graph plot type, linear or bar
kLinearPlot <- 1
kBarPlot <- 0

# Platforms string names
kApp.types <- c(kApp.Sobel, kApp.FFT)
platforms <- c("CPU","GPU")

# Platforms code
kCPU <- 1
kGPU <- 2

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
graph.xlabel <- "Dimensão das amostras [px]"
graph.pch <- c(15,16,17)
graph.font.label <- 2
graph.lwd <- 3
graph.plot.type <- "o"
graph.line.types <- c("solid", "dotted", "dashed")
graph.colors <- c("#E41A1C", "#377EB8","#4DAF4A")
graph.axis.at <- c(256, 512, 1024, 2048, 4096, 8192)
#graph.axis.labels <- c("256x256", "512x512", "1024x1024", "2048x2048", "4096x4096", "8192x8192")
graph.legend.title.gpu <- "Modelo de GPU"
graph.legend.values.gpu <- c("NVIDIA GT 520", "NVIDIA GT 210", "ATI HD 6450")
graph.legend.title.cpu <- "Modelo de CPU"
graph.legend.values.cpu <- c("Intel Core 2 Duo - 1", "Intel Core 2 Duo - 2", "Intel Core 2 Duo - 3")


# the title of graphs to plot
graph.titles <- list(
c("Tempo Médio de execução do kernel", "Tempo médio em milisegundos", kMean, kLinearPlot),
c("Desvio Padrão do tempo médio", "Tempo em milisegundos", kSd, kLinearPlot),
c("Variância do tempo médio", "Tempo em milisegundos", kVar, kLinearPlot),
c("Tempo Médio de execução total", "Tempo médio em segundos", kMean, kLinearPlot),
c("Desvio Padrão do tempo total", "Tempo em segundos", kSd, kLinearPlot ),
c("Variância do tempo total", "Tempo em milisegundos", kVar, kLinearPlot),
c("Taxa Média de transferência do host para o device", "Taxa média de transferência em MB/s", kMean, kBarPlot),
c("Taxa Média de transferência do device para o host", "Taxa média de transferência em MB/s", kMean, kBarPlot),
c("Desvio Padrão da taxa transferência do host para o device", "Taxa de transferência em MB/s", kSd, kLinearPlot),
c("Desvio Padrão da taxa transferência do device para o host", "Taxa de transferência em MB/s", kSd, kLinearPlot),
c("Variância da taxa transferência do host para o device", "Taxa de transferência em MB/s", kVar, kLinearPlot),
c("Variância da taxa transferência do device para o host", "Taxa de transferência em MB/s", kVar, kLinearPlot))

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
  for (platform.type in 1:2 ) {
    if (platform.type == kCPU) {
      graph.total <- 4
      graph.legend.title <- graph.legend.title.cpu
      graph.legend.values <- graph.legend.values.cpu
      if (app.type == kSobel){
        graph.data <- list(cpu.520.sobel.data,
                       cpu.210.sobel.data,
                       cpu.6450.sobel.data)
        } else {
          graph.data <- list(cpu.520.fft.data,
                       cpu.210.fft.data,
                       cpu.6450.fft.data)
        }
    } else {
      graph.total <- 8
      graph.legend.title <- graph.legend.title.gpu
      graph.legend.values <- graph.legend.values.gpu

      if (app.type == kSobel){
        graph.data <- list(nvidia.520.sobel.data,
                       nvidia.210.sobel.data,
                       ati.6450.sobel.data)
        } else {
          graph.data <- list(nvidia.520.fft.data,
                       nvidia.210.fft.data,
                       ati.6450.fft.data)
        }
    }

    for (graph.choice in 1:graph.total) {
    if (platform.type == kCPU) {
      column.data <- switch(
      toString(graph.choice),
      "1" = kTempoKernel.cpu,
      "2" = kTempoTotal.cpu,
      "3" = kTempoKernel.cpu,
      "4" = kTempoTotal.cpu)

    } else {
      column.data <- switch(
      toString(graph.choice),
      "1" = kTempoKernel.gpu,
      "2" = kTempoTotal.gpu,
      "3" = kTempoKernel.gpu,
      "4" = kTempoTotal.gpu,
      "5" = kCopyHost2Dev,
      "6" = kCopyDev2Host,
      "7" = kCopyHost2Dev,
      "8" = kCopyDev2Host)
    }

    graph.type <- as.integer(graph.titles[[graph.choice]][4])
    graph.metric <- as.integer(graph.titles[[graph.choice]][3])
    graph.ylabel <- graph.titles[[graph.choice]][2]
    graph.title <- paste(platforms[platform.type], kApp.types[app.type], graph.titles[[graph.choice]][1], sep=" - ")

    graph.xlim <- c(128, 8196)

    pdf.file = paste(paste(kGraph.path, graph.title, sep="\\"), ".pdf")

    pdf(pdf.file, width=9, height=7) # in inch
    # Allocate a vector with 6 spaces
    data.resolucao <- rep(NA, 6)
    data.520 <- rep(NA, 6)
    data.210 <- rep(NA, 6)
    data.6450 <- rep(NA, 6)

    if (graph.metric == kMean) {
      # loop to calculate the average of 6 samples in column data
      for (n in 1:6) {
        data.resolucao[n] = mean(graph.data[[3]][[kResolucao]][((n-1)*8 + 1):(n*8)])
        data.520[n] = mean(graph.data[[1]][[column.data]][((n-1)*8 + 1):(n*8)])
        data.210[n] = mean(graph.data[[2]][[column.data]][((n-1)*8 + 1):(n*8)])
        data.6450[n] = mean(graph.data[[3]][[column.data]][((n-1)*8 + 1):(n*8)])
      }
    } else if( graph.metric == KSd) {
           # loop to calculate the standard deviation of 6 samples in column data
           for (n in 1:6) {
             data.resolucao[n] = mean(graph.data[[3]][[kResolucao]][((n-1)*8 + 1):(n*8)])
             data.520[n] = sd(graph.data[[1]][[column.data]][((n-1)*8 + 1):(n*8)])
             data.210[n] = sd(graph.data[[2]][[column.data]][((n-1)*8 + 1):(n*8)])
             data.6450[n] = sd(graph.data[[3]][[column.data]][((n-1)*8 + 1):(n*8)])
           }
          } else {
              for (n in 1:6) {
                 data.resolucao[n] = mean(graph.data[[3]][[kResolucao]][((n-1)*8 + 1):(n*8)])
                 data.520[n] = var(graph.data[[1]][[column.data]][((n-1)*8 + 1):(n*8)])
                 data.210[n] = var(graph.data[[2]][[column.data]][((n-1)*8 + 1):(n*8)])
                 data.6450[n] = var(graph.data[[3]][[column.data]][((n-1)*8 + 1):(n*8)])
                 }
           }

    # errpad.520 <- sd(data.520)/sqrt(length(data.520))
    # iconfianca.520 <- errpad.520

    # errpad.210 <- sd(data.210)/sqrt(length(data.210))
    # iconfianca.210 <- errpad.210

    # errpad.6450 <- sd(data.6450)/sqrt(length(data.6450))
    # iconfianca.6450 <- errpad.6450

    # iconfianca <- c(iconfianca.520, iconfianca.210, iconfianca.6450)

    max.ylim <- max(data.210, na.rm=TRUE)

    if (max(data.6450) > max.ylim) {
        max.ylim <- max(data.6450)
    }

    if (max(data.520, na.rm=TRUE) > max.ylim) {
      max.ylim <- max(data.520, na.rm=TRUE)
    }

    graph.ylim <- c(0, max.ylim*(1.3))

    if ( graph.type == kLinearPlot){
    plot(x        = data.resolucao,
         y        = data.210,
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
         cex.axis = 0.8)

    points(x    = data.resolucao,
           y    = data.520,
           type = graph.plot.type,
           pch  = graph.pch[1],
           col  = graph.colors[1],
           lwd  = graph.lwd,
           lty  = graph.line.types[1])

    points(x    = data.resolucao,
           y    = data.6450,
           type = graph.plot.type,
           pch  = graph.pch[3],
           col  = graph.colors[3],
           lwd  = graph.lwd,
           lty  = graph.line.types[3])

    abline(v=c(256, 512, 1024, 2048, 4096, 8192), col="gray", lty="dotted")
    abline(h=(seq(0, max.ylim,  max.ylim/10)), col="gray", lty="dotted")

    axis(side     = 1,
         at       = graph.axis.at,
         labels   = expression(2^16, 2^18, 2^20, 2^22, 2^24, 2^26),
    #     labels   = graph.axis.labels,
         cex.axis = 0.8)

    title(graph.title)

    legend(x = "topleft",
           title = graph.legend.title, graph.legend.values,
           col   = graph.colors,
           lty   = graph.line.types,
           lwd   = graph.lwd,
           pch   = graph.pch,
           xpd   = T,
           bg = "white")

    add.error.bars <- function(X, Y, SE, w, col)
    {
      X0 = X; Y0 = (Y-SE); X1=X; Y1 = (Y+SE);
      arrows(X0, Y0, X1, Y1, code=3, angle=90, length=w, col=col);
    }

    # add.error.bars(data.resolucao, data.520,  iconfianca.520, 0.1, col=graph.colors[1]);
    # add.error.bars(data.resolucao, data.210,  iconfianca.210, 0.1, col=graph.colors[2]);
    # add.error.bars(data.resolucao, data.6450, iconfianca.6450, 0.1, col=graph.colors[3]);

     } else {

    data.barplot <- matrix(c(data.520, data.210, data.6450), ncol=6, byrow=TRUE)
    colnames(data.barplot) <- data.resolucao
    data.barplot <- as.table(data.barplot)

    taxas <- barplot(height    = as.matrix(data.barplot),
            xlab      = graph.xlabel,
            ylab      = graph.ylabel,
            font.lab  = graph.font.label,
            beside    = T,
            col       = graph.colors,
            names.arg = expression(2^16, 2^18, 2^20, 2^22, 2^24, 2^26),
    #        names.arg = graph.axis.labels,
            ylim      = graph.ylim)


    #arrows(taxas, data.barplot-iconfianca, centros, data.barplot+iconfianca, length=0.1, angle=90, code=3)

    title(graph.title)

    legend(x     = "topleft",
           title = graph.legend.title, graph.legend.values,
           fill  = graph.colors,
           bty   = "n",
           horiz = T)
    }

    dev.off()

    rm(data.520)
    rm(data.210)
    rm(data.6450)

      }
     }
}