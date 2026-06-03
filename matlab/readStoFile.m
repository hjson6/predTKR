function data = readStoFile(filename, lineToStart)
    % Read the .sto file using importdata
    try
        dataStruct = importdata(filename, '\t', lineToStart);
    catch
        error('Error reading data from the file.');
    end
    
    % Check if the data contains numeric values
    if ~isstruct(dataStruct) || ~isfield(dataStruct, 'data') || ~isnumeric(dataStruct.data)
        error('Error: No numeric data found in the file.');
    end
    
    % Extract the numeric data from the structure and return as a single array
    data = dataStruct.data;
end
