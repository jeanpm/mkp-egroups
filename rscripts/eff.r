library(extrafont)
loadfonts(quiet=TRUE)

file = "efficiencies/30.100"

t = read.table(file)

par(mfrow=c(1,3))

first = 0

plotar = function(t, col, dc)
{
    x = apply(t, 2, mean)
    x = (x - min(x))/(max(x) - min(x))
    x = round(x, dc)
    plot(x, col=col, lwd=1, las=1, ylab='Efficiency', xlab='Item', pch=3)
}

Width=4
Height=3

Margins = c(4,4,1,1)

t = t[1,]
file = "figs/dcases=0.pdf"
pdf(file, family="CM Roman", width=Width, height=Height)
par(mar=Margins)
plotar(t, 1, 10)
dev.off()
embed_fonts(file)

dc = 3
file = "figs/dcases=3.pdf"
pdf(file, family="CM Roman", width=Width, height=Height)
par(mar=Margins)
plotar(t, 1, dc)
dev.off()
embed_fonts(file)

dc = 2
file = "figs/dcases=2.pdf"
pdf(file, family="CM Roman", width=Width, height=Height)
par(mar=Margins)
plotar(t, 1, dc)
dev.off()
embed_fonts(file)

dc = 1
file = "figs/dcases=1.pdf"
pdf(file, family="CM Roman", width=Width, height=Height)
par(mar=Margins)
plotar(t, 1, dc)
dev.off()
embed_fonts(file)

dev.off()

