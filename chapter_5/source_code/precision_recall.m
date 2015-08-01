%******************************************************************************************************
% This code is part of the code supplied with the OpenCV Blueprints book.
% It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
%
% License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
% *****************************************************************************************************

% Script for visualizing OpenCV precision recall data
% Expects the data in a textfile with the following structure
% x-coordinate y-coordinate associated-threshold

close all;
clear all;

% Set the data location and the delimiter seperating the data
filename = '/data/test/coordinates.txt';
delimeter = ' ';
headerLines = 0;

% Read in the data
data = importdata(filename,delimeter,headerLines);
precision = data(:,1);
recall = data(:,2);
scores = data(:,3);

% Visualize the curve
figure();
plot(recall, precision);
axis([0 1 0 1]);
title('Precision-Recall curve');
xlabel('Recall');
ylabel('Precision');

% Append two data points to connect towards the axis
% Else a wrong AUC is calculated!
precision = [0; precision; precision(end,1)];
recall = [recall(1,1); recall; 0];

% Calculate the area under curve
AUC = trapz(recall, precision);
AUC = abs(AUC) * 100;
display = ['Area under curve = ' num2str(AUC) '%'];
text(50, 50, display, 'units', 'pixels');
