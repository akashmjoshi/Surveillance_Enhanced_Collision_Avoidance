clear all
clc

R = 80;
dy = [0:(2*R)];
dx = [0:(2*R)];
dy = dy.^2;
dx = dx.^2;

for i = 1:numel(dx)
    for j = 1:numel(dy)
        dis(i,j)=dx(i)+dy(j);
    end
end

dis= unique(dis);
dis(find(dis==0))=[];
d = sqrt(dis);
d(find(d>(2*R))) =[];
theta = acos((d./2)/R);
S = pi*(R^2);
per = 4*(S*(theta./(2*pi))-(R/2)*(d./2).*sin(theta))/S;

list(:,1)=d.^2;
list(:,2)=per;

plot(d,per.*100);
title('Predicted distance VS. Collission possibility');
xlabel('Predicted distance');
ylabel('Collission possibility')
