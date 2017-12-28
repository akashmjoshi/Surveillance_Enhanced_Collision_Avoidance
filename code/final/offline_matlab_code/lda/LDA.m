clc;close all;
%control variable
ycbcr = 0;
cbcr_only = 1;
pca_reduction = 0;
loc_add = 0;
loc_only = 0;
%load training image
train_im = imread('car1.png');
% train_im = imread('car_yy.png');
% test_im = imread('car_test_yy.png');
test_im = imread('car.jpg');
train = im2double(train_im);
test = im2double(test_im);
if(ycbcr == 1)
    train = rgb2ycbcr(train);
    test = rgb2ycbcr(test);
end
if(cbcr_only == 1)
    train = rgb2ycbcr(train);
%     train(:,:,3) = [];
%     train(:,:,2) = [];
    train(:,:,1) = [];
    test = rgb2ycbcr(test);
%     test(:,:,3) = [];
%     test(:,:,2) = [];
    test(:,:,1) = [];
end

[train_rows,train_cols,train_depth] = size(train);
train = reshape(train,[train_rows*train_cols,train_depth]);
[test_rows,test_cols,test_depth]=size(test);
test = reshape(test,[test_rows*test_cols,test_depth]);
% generate location
if(loc_add == 1)
    train_loc_i = ones(train_rows,train_cols);
    for i = 1:train_rows
        train_loc_i(i,:) = i;
    end
    train_loc_j = ones(train_rows,train_cols);
    for j = 1:train_cols
        train_loc_j(:,j) = j;
    end
    train_loc_i = reshape(train_loc_i, [train_rows*train_cols,1]);
    train_loc_j = reshape(train_loc_j, [train_rows*train_cols,1]);
    train = [train, train_loc_i, train_loc_j];
    %testing image
    test_loc_i = ones(test_rows,test_cols);
    for i = 1:test_rows
        train_loc_i(i,:) = i;
    end
    test_loc_j = ones(test_rows,test_cols);
    for j = 1:test_cols
        test_loc_j(:,j) = j;
    end
    test_loc_i = reshape(test_loc_i, [test_rows*test_cols,1]);
    test_loc_j = reshape(test_loc_j, [test_rows*test_cols,1]);
    test = [test, test_loc_i, test_loc_j];
end
if(pca_reduction == 1)
    [coeff,~,latent] = pca(train);
    train = train*coeff(:,1:2);
    [coeff,~,latent] = pca(test);
    test = test*coeff(:,1:2);
end

if(loc_only == 1)
   train = [train_loc_i, train_loc_j];
   test = [test_loc_i, test_loc_j];
end

%load labelled image
train_label = imread('car_label.png');
label = mean(train_label,3); 
obj1 = find(label == 255);
obj2 = find(label == 127);
label = ones(train_rows,train_cols);
label(obj1) = 2;
label(obj2) = 3;
label = reshape(label,[train_rows*train_cols,1]);
%train/fitting model on data
model = fitcdiscr(train,label);
out = predict(model,test);
out_im = reshape(out,[test_rows,test_cols]);
%projection matrix from LDA
[LTrans,Lambda] = eig(model.BetweenSigma,model.Sigma,'chol');
[Lambda,sorted] = sort(diag(Lambda),'descend'); % sort by eigenvalues
LTrans = LTrans(:,sorted);
Xtransformed = model.XCentered*LTrans;
% plotting ??
K_2_3 = model.Coeffs(2,3).Const;
L_2_3 = model.Coeffs(3,2).Linear;
K_1_2 = model.Coeffs(1,2).Const;
L_1_2 = model.Coeffs(2,1).Linear;
K_1_3 = model.Coeffs(1,3).Const;
L_1_3 = model.Coeffs(3,1).Linear;
% z = -1:0.01:1; 
% [x_mesh,y_mesh,z_mesh] = meshgrid(x,y,z);
% f =  K + L(1)*x_mesh + L(2)*y_mesh;
f_2_3 = @(x1,x2) K_2_3 + L_2_3(1)*x1 + L_2_3(2)*x2;
f_1_2 = @(x1,x2) K_1_2 + L_1_2(1)*x1 + L_1_2(2)*x2;
f_1_3 = @(x1,x2) K_1_3 + L_1_3(1)*x1 + L_1_3(2)*x2;

% sliceomatic(f,x,y,z)
obj0 = (1:(train_rows*train_cols))';
obj0([obj1;obj2]) = [];


% bg_org = model.XCentered(obj0,:);
% data1_org = model.XCentered(obj1,:);
% data2_org = model.XCentered(obj2,:);

train_org = reshape(train_im,[train_rows*train_cols,3]);
if(ycbcr == 1)
    train_org = rgb2ycbcr(train_org);
end
bg_org = train_org(obj0,:);
data1_org = train_org(obj1,:);
data2_org = train_org(obj2,:);

bg_trans = Xtransformed(obj0,:);
data1_trans = Xtransformed(obj1,:);
data2_trans = Xtransformed(obj2,:);
% train_im = reshape(train_im,size(train));
% bg = train_im(obj0,:);
% data1 = train_im(obj1,:);
% data2 = train_im(obj2,:);
% plot3(Xtransformed(:,1),Xtransformed(:,2),Xtransformed(:,3),'.');
% plot3(model.XCentered(:,1),model.XCentered(:,2),model.XCentered(:,3),'.');

train_depth = size(train,2);
if(train_depth == 2 || pca_reduction == 1)
    figure;
    hold on;
%     h_2_3 = ezplot(f_2_3,[-0.2 0.2 -0.3 0.3]);    
%     h_1_3 = ezplot(f_1_3,[-0.2 0.2 -0.3 0.3]);
%     h_1_2 = ezplot(f_1_2,[-0.2 0.2 -0.3 0.3]);  
    
    h_2_3 = ezplot(f_2_3,[0 255 0 255]);   
%     h_1_3 = ezplot(f_1_3,[0 255 0 255]);
%     h_1_2 = ezplot(f_1_2,[0 255 0 255]);
%     h_1_3.Color = 'y';
%     h_1_2.Color = 'k';
%     plot(bg_org(:,1),bg_org(:,2), ['.','g']);
    plot(data1_org(:,1),data1_org(:,2), ['.','r']);
    plot(data2_org(:,1),data2_org(:,2), ['.','b']);
    title('Orignal space');
    legend('Object Boundary','Orange car','Blue car');
%     legend('Background','Orange car','Blue car');
%     legend('Object boundary','Boundary - orange car','Boundary - Blue car','Background','Orange car','Blue car');
    hold off;
    figure;
    hold on;
%     h_2_3 = ezplot(f_2_3,[-30 30 -25 25]);    
%     h_1_3 = ezplot(f_1_3,[-30 30 -25 25]);
%     h_1_2 = ezplot(f_1_2,[-30 30 -25 25]);
%     h_1_3.Color = 'y';
%     h_1_2.Color = 'k';
    plot(bg_trans(:,1),bg_trans(:,2), ['.','g']);
    plot(data1_trans(:,1),data1_trans(:,2), ['.','r']);
    plot(data2_trans(:,1),data2_trans(:,2), ['.','b']);
    title('Transformed space');
    legend('Background','Orange car','Blue car');
%     legend('Object boundary','Boundary - orange car','Boundary - Blue car','Background','Orange car','Blue car');
    hold off;
elseif(train_depth == 3)
    hold on;
    plot3(bg_org(:,1),bg_org(:,2),bg_org(:,3), ['.','g']);
    plot3(data1_org(:,1),data1_org(:,2),data1_org(:,3), ['.','r']);
    plot3(data2_org(:,1),data2_org(:,2),data2_org(:,3), ['.','b']);
    legend('Background','Orange car','Blue car');
    hold off;
    figure;
    hold on;
    plot3(bg_trans(:,1),bg_trans(:,2),bg_trans(:,3), ['.','g']);
    plot3(data1_trans(:,1),data1_trans(:,2),data1_trans(:,3), ['.','r']);
    plot3(data2_trans(:,1),data2_trans(:,2),data2_trans(:,3), ['.','b']);
    legend('Background','Orange car','Blue car');
    hold off;
end
figure;
imagesc(out_im);


