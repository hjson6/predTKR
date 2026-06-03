function [time, data] = readMotFile(filename)
    % This function reads a .mot file and extracts time and data
    % Inputs:
    %   filename - the name of the .mot file
    % Outputs:
    %   time - a vector of time values
    %   data - a matrix of motion data (each column corresponds to a variable)

    % Open the file for reading
    fid = fopen(filename, 'r');
    if fid == -1
        error('Error opening the file: %s', filename);
    end

    % Skip the first 21 lines to reach the data
    for i = 1:21
        fgetl(fid);
    end

    % Initialize an empty array to hold data
    data = [];

    % Read the data starting from row 22
    while ~feof(fid)
        line = fgetl(fid);
        if ischar(line)  % Ensure line is a character array
            values = sscanf(line, '%f');
            if ~isempty(values)  % Ensure there are values
                data = [data; values'];  % Append row of values
            end
        end
    end

    % Close the file
    fclose(fid);

    % Check if data is empty
    if isempty(data)
        error('No data read from the file. Please check the file format and content.');
    end

    % Extract time and state data
    time = data(:, 1);  % Assuming the first column is time
    data = data(:, 2:end);  % The rest are state data

    % Return the results
    disp('Data read successfully.');
end