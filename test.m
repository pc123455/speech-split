[x, fs, nbits]=wavread('time(0700-0800)_011111_01_01.wav');
%新的采样率
newfs = fs;
%左右声道计算均值
x = mean(x,2);
%重采样
x = resample(x,newfs,fs);
%去掉直流分量
x = x - mean(x);
%窗长度
window_size = 0.2*newfs;
%短时能量
energy = [];
%短时能量阈值
threshold = 0.5;
for i = 1:length(x) - window_size
    energy(end+1) = sum(x(i:i+window_size).^2);
end
%最短间隔
min_space_length = 1*newfs;
%边界点
bounds = [0];
for i = 1:length(energy)
    if energy(i) < threshold && bounds(end) < (i + round(window_size/2))/newfs - 1
        bounds(end+1) = (i + round(window_size/2))/newfs;
    end
end
%wavplay(x);