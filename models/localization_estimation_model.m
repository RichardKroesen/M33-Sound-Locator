
clf("reset");

xlim([0, 500]);
ylim([0, 500]);

% Test arguments
buzz_pos = [350, 120];
buzz_radius = 1;
buzz_speed = 600;

% Sensor config
sensors = struct( ...
    "Pos", {[50, 50], [450, 90], [420, 380]}, ...
    "SensedAt", {0, 0, 0} ...
);
all_sensed = false;

% Code, don't touch
buzz_graphic = drawcircle( ...
    Center = buzz_pos, ...
    Radius = buzz_radius ...
);
buzz_graphic.FaceAlpha = 0;
% Actual source
drawcircle( ...
    Center = buzz_pos, ...
    Radius = 10, ...
    Color = [1, 1, 0], ...
    FaceAlpha = 0 ...
);

for i = 1:3
    drawcircle( ...
        Center = sensors(i).Pos, ...
        Radius = 10, ...
        Color = "r" ...
    )
end

function on_all_sensed(sensors, buzz_speed, buzz_pos) %#codegen
    [~, sorted] = sort([sensors.SensedAt]);
    sensors = sensors(sorted);
    
    d12 = (sensors(2).SensedAt - sensors(1).SensedAt) * buzz_speed;
    d13 = (sensors(3).SensedAt - sensors(1).SensedAt) * buzz_speed;

    x1 = sensors(1).Pos(1);
    x2 = sensors(2).Pos(1);
    x3 = sensors(3).Pos(1);
    y1 = sensors(1).Pos(2);
    y2 = sensors(2).Pos(2);
    y3 = sensors(3).Pos(2);

    f = @(p) [
        sqrt((p(1) - x2)^2 + (p(2) - y2)^2) - sqrt((p(1) - x1)^2 + (p(2) - y1)^2) - d12;
        sqrt((p(1) - x3)^2 + (p(2) - y3)^2) - sqrt((p(1) - x1)^2 + (p(2) - y1)^2) - d13;
    ];

    init_guess = [mean([x1, x2, x3]), mean([y1, y2, y3])];

    solve_options = optimoptions("fsolve", "FunctionTolerance", 1e-6, "StepTolerance", 1e-6);
    source_position = fsolve(f, init_guess, solve_options);
    
    drawcircle( ...
        Center = source_position, ...
        Radius = 10, ...
        Color = [0, 0, 1], ...
        FaceAlpha = 0 ...
    )

    disp("d12 and d13");
    disp(d12);
    disp(d13);

    disp(sensors(1).Pos);
    disp(sensors(2).Pos);
    disp(sensors(3).Pos);

    disp(init_guess);

    disp("Margin of error (px):");
    disp(abs(source_position - buzz_pos));
end

for time = 0:0.016:1
    buzz_graphic.Radius = time * buzz_speed;
    
    sensed = 0;
    for i = 1:3
        if sensors(i).SensedAt == 0 && inROI(buzz_graphic, sensors(i).Pos(1), sensors(i).Pos(2))
            sensors(i).SensedAt = time;
        end
        if sensors(i).SensedAt > 0
            sensed = sensed + 1;
        end
    end
    if all_sensed == false && sensed == 3
        all_sensed = true;
        on_all_sensed(sensors, buzz_speed, buzz_pos);
    end

    pause(0.016)
end

