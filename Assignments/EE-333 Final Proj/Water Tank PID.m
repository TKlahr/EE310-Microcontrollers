clear; clc; close all;

%% Simulation Settings
dt = 0.1;
t = 0:dt:120;

target = 50;                 % desired tank level (%)

%% PID Gains
Kp = 0.7;
Ki = 0.025;
Kd = 0.18;

%% System Variables
level = zeros(size(t));
level(1) = 50;

pump = zeros(size(t));
demand = zeros(size(t));

integral = 0;
prev_error = 0;

pump_max = 20;

%% Figure Setup
figure;

axis([0 4 0 110]);
hold on;
grid on;

% Tank outline
rectangle('Position',[1 0 2 100], ...
          'EdgeColor','k', ...
          'LineWidth',3);

% Water level graphic
water = rectangle('Position',[1 0 2 level(1)], ...
                  'FaceColor','b');

% Text displays
levelText = text(2,105,'', ...
    'HorizontalAlignment','center', ...
    'FontSize',14);

eventText = text(2,95,'', ...
    'HorizontalAlignment','center', ...
    'Color','r', ...
    'FontSize',12);

title('PID Controlled Factory Water Tank');
ylabel('Tank Level (%)');

%% Simulation Loop
for k = 2:length(t)

    % Factory demand spike every 10 seconds
    if mod(round(t(k)),10) == 0 && abs(mod(t(k),10)) < dt

        % Random factory demand spike
        demandSpike = randi([15 50]);

        demand(k) = demandSpike;

        eventText.String = sprintf( ...
        'Factory Demand: -%d%%', demandSpike);

    else
        demand(k) = 2;
        eventText.String = '';
    end

    % PID Error
    error = target - level(k-1);

    integral = integral + error*dt;

    % Anti-windup: limit integral buildup
    integral = max(-80, min(integral, 80));

    derivative = (error - prev_error)/dt;

    % PID Controller
    pump(k) = Kp*error + Ki*integral + Kd*derivative;

    % Pump limits
    pump(k) = max(0, min(pump(k), pump_max));

    % Tank dynamics
    level(k) = level(k-1) + (pump(k) - demand(k))*dt;

    % Limit level range
    level(k) = max(0, min(level(k),100));

    prev_error = error;

    %% Update Tank Graphic
    water.Position = [1 0 2 level(k)];

    levelText.String = sprintf( ...
        'Water Level: %.1f%%', level(k));

    drawnow;

    pause(0.03);
end

%% Final Response Plot
figure;
plot(t,level,'LineWidth',2);
hold on;
yline(target,'r--','Target Level');

xlabel('Time (s)');
ylabel('Tank Level (%)');

title('Tank Level Response');

grid on;
