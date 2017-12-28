clear all

fid = fopen('buffer_out_by_car.dat');
result = fscanf(fid,'%x');
data = result(6:end);
y1 = bitand(data,hex2dec('FF000000'));
y1 = bitshift(y1,-24);

y0 = bitand(data,hex2dec('0000FF00'));
y0 = bitshift(y0,-8);

cr = bitand(data,hex2dec('00FF0000'));
cr = bitshift(cr,-16);

cb = bitand(data,hex2dec('000000FF'));
% figure(1)
% plot3(y0,cb,cr,'.');
% xlabel('y');
% ylabel('cb');
% zlabel('cr');

% figure(1);
% histogram(y1);
% xlim([0,255]);
% title('y1');
% 
% figure(2);
% histogram(y0);
% xlim([0,255]);
% title('y0');
% 
% figure(3);
% histogram(cb);
% xlim([0,255]);
% title('cb');
% 
% figure(4);
% histogram(cr);
% xlim([0,255]);
% title('cr');

% figure(2)
 y = zeros(345600,1);
 y(1:2:end)= y1';
 y(2:2:end)= y0';
 
 Cb = zeros(345600,1);
 Cb(1:2:end)= cb';
 Cb(2:2:end)= cb';
 
 Cr = zeros(345600,1);
 Cr(1:2:end)= cr';
 Cr(2:2:end)= cr';
 
 im = zeros(480,720,3);
%  
%  y = (reshape(y,720,480)')./255;
%  Cb = (reshape(Cb,720,480)')./255;
%   Cr = (reshape(Cr,720,480)')./255;
% 
  y = (reshape(y,720,480)');
  Cb = (reshape(Cb,720,480)');
  Cr = (reshape(Cr,720,480)');

% 
%   
  im(:,:,1) = y;
 
  im(:,:,2) = Cb;
  im(:,:,3) = Cr; 
%   imshow(ycbcr2rgb(im));
%   
  LABEL = ones(480,720);
  
  LABEL(240:294,250:283)=2;
  LABEL(252:281,369:404)=3;
  
%   
  figure(3);
  
  a = 0.0016;
  b = -0.0712;
  c = -0.2549;
% a=a/norm([a b c]);
% 
% b=b/norm([a b c]);
% 
% c=c/norm([a b c]);

  subplot(3,1,1);
  histogram(a*y(find(LABEL ==1))+b*Cb(find(LABEL ==1))+c*Cr(find(LABEL ==1)));
  
   subplot(3,1,2);
  histogram(a*y(find(LABEL ==2))+b*Cb(find(LABEL ==2))+c*Cr(find(LABEL ==2)));
  
   subplot(3,1,3);
  histogram(a*y(find(LABEL ==3))+b*Cb(find(LABEL ==3))+c*Cr(find(LABEL ==3)),[0:5:255]);
  
  cluster1 = a*y(find(LABEL ==1))+b*Cb(find(LABEL ==1))+c*Cr(find(LABEL ==1));
  cluster2 = a*y(find(LABEL ==2))+b*Cb(find(LABEL ==2))+c*Cr(find(LABEL ==2));
  cluster3 = a*y(find(LABEL ==3))+b*Cb(find(LABEL ==3))+c*Cr(find(LABEL ==3));
  
  fprintf('Mean 1 = %f, var 1 = %f\n',[mean(cluster1),var(cluster1)]);
fprintf('mean 2 = %f, var 2 = %f\n',[mean(cluster2), var(cluster2)]);
fprintf('mean 3 = %f, var 3 = %f\n',[mean(cluster3), var(cluster3)]);
  
%    
%    
%   
%   
%  
%  
%  
%          
% 
% 
