%{ MIT License
 
 Copyright (c) 2025 Feb. by Julian Bruin, James Schutte, Richard Kroesen
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
%}  SOFTWARE.

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

