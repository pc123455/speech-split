[x, fs, nbits]=wavread('time(0700-0800)_011111_01_01.wav');
%�µĲ�����
newfs = fs;
%�������������ֵ
x = mean(x,2);
%�ز���
x = resample(x,newfs,fs);
%ȥ��ֱ������
x = x - mean(x);
%������
window_size = 0.2*newfs;
%��ʱ����
energy = [];
%��ʱ������ֵ
threshold = 0.5;
for i = 1:length(x) - window_size
    energy(end+1) = sum(x(i:i+window_size).^2);
end
%��̼��
min_space_length = 1*newfs;
%�߽��
bounds = [0];
for i = 1:length(energy)
    if energy(i) < threshold && bounds(end) < (i + round(window_size/2))/newfs - 1
        bounds(end+1) = (i + round(window_size/2))/newfs;
    end
end
%wavplay(x);