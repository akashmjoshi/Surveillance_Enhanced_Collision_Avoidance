clear all;
fid = fopen('buffer_out_by_car.dat','r');
raw_data = textscan(fid, '%s');
fclose(fid);
data = cell2mat(raw_data{1}( 6:end));
ycbcr_data = hex2dec(data(1:end,3:10));
cr = bitshift(bitand(ycbcr_data, 16711680*ones(172800,1)),-16);
cb = bitand(ycbcr_data, 255*ones(172800,1));
y0 = bitshift(bitand(ycbcr_data, 65280*ones(172800,1)),-8);
y1 = bitshift(bitand(ycbcr_data, 4278190080*ones(172800,1)),-24);
  
% figure;
% subplot(3,1,1)
% histogram(cb)
% subplot(3,1,2)
% histogram(cr)
% subplot(3,1,3)
% histogram(y0)
%colors = [cb,cr];
% 
% y0(find(y0<16)) =  y0(find(y0<16))+ 238;
% data = [cr./255, cb./255, y0./255];
% gmmodel = fitgmdist(data,3);
% idx = cluster(gmmodel,data);
% cluster1 = data(idx == 1,:);
% cluster2 = data(idx == 2,:);
% cluster3 = data(idx == 3,:);
% fprintf('Mean 1 = %f, var 1 = %f\n',[mean(cluster1),var(cluster1)]);
% fprintf('mean 2 = %f, var 2 = %f\n',[mean(cluster2), var(cluster2)]);
% fprintf('mean 3 = %f, var 3 = %f\n',[mean(cluster3), var(cluster3)]);

training_label = ones(480,720);
training_label(240:294,250:283) = 2;
training_label(252:281,369:404) = 3;
label = reshape(training_label,[345600,1]);
y = zeros(345600,1);
y(1:2:end)= y1';
y(2:2:end)= y0';
Cb = zeros(345600,1);
Cb(1:2:end)= cb';
Cb(2:2:end)= cb';
Cr = zeros(345600,1);
Cr(1:2:end)= cr';
Cr(2:2:end)= cr';
input_data = [y,Cb,Cr];
Mdl = fitcdiscr(input_data,label)
out = predict(Mdl, input_data);
% figure;
% imagesc(training_label)
% figure;
% subplot(4,1,1)
% histogram(Cr(find(training_label == 1)))
% subplot(4,1,2)
% histogram(Cr(find(training_label == 2)))
% subplot(4,1,3)
% histogram(Cr(find(training_label == 3)))
% subplot(4,1,4)
% histogram(out)

% 
% r = zeros(172800,1);
% g = zeros(172800,1);
% b = zeros(172800,1);
% r(idx == 1)  = 235;
% g(idx == 2)  = 235;
% b(idx == 3)  = 235;

R = zeros(345600,1);
G = zeros(345600,1);
B = zeros(345600,1);
R(out == 1) = 235;
G(out == 2) = 235;
B(out == 3) = 235;

% R(1:2:end)= r';
% R(2:2:end)= r';
% G(1:2:end)= g';
% G(2:2:end)= g';
% B(1:2:end)= b';
% B(2:2:end)= b';
cluster_im = zeros(480,720,3);
R = (reshape(R,720,480)')./255;
G = (reshape(G,720,480)')./255;
B = (reshape(B,720,480)')./255;
cluster_im(:,:,1) = R;
cluster_im(:,:,2) = G;
cluster_im(:,:,3) = B;
figure
imshow(cluster_im)
% 
% im = zeros(480,720,3);
% y = (reshape(y,720,480)')./255;
% Cb = (reshape(Cb,720,480)')./255;
% Cr = (reshape(Cr,720,480)')./255;
% im(:,:,1) = y;
% im(:,:,2) = Cb;
% im(:,:,3) = Cr;
% figure
% imshow(ycbcr2rgb(im));