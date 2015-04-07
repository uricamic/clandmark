function [ T ] = flandmark_xmlread( data )
%FLANDMARK_XMLREAD Summary of this function goes here
%   Detailed explanation goes here
%
% 10-04-2013 Michal Uricar

    T = [];

    if( strcmp(class(data),'org.apache.xerces.dom.DeferredElementImpl') )
        docNode = data;
    else
        %% Convert XML-file to the current format if necessary
        % load the file lines
        fid = fopen(data,'r');
        nl = 0; line = [];
        while ~feof(fid)
            nl = nl + 1;
            line{nl} = fgetl(fid);
        end;
        fclose(fid);
        % check the file format 
        % remove the second line "<!DOCTYPE ..." if necessary
        if (length(line)>1) && strcmp(line{2}(1:min(9,length(line{2}))),'<!DOCTYPE')
            fid = fopen(data,'w');
            for jj = 1:nl
                if strcmp(line{jj}(1:min(9,length(line{jj}))),'<!DOCTYPE')
                    continue;
                end;
                fprintf(fid,'%s\n',line{jj});
            end;
            fclose(fid);
            fprintf('File %s converted to the current xml-format.\n',data);
        end;
        
        %% load the file in the current format
        docNode = xmlread( data );

        %% 
        
        T.name = docNode.getElementsByTagName('name').item(0).getTextContent.toCharArray';
        T.version = docNode.getElementsByTagName('version').item(0).getTextContent.toCharArray';
        
        %% Get "options"
        xmlList = docNode.getElementsByTagName('num_nodes');
        xmlEl = xmlList.item(0);
        T.numNodes = str2num(xmlEl.getTextContent);
        
        T.bw = zeros(2, 1);
        T.bw_margin = zeros(2, 1);
        T.compnames = cell(T.numNodes, 1);
        T.components = zeros(2, T.numNodes);
        T.ss = zeros(4, T.numNodes);

        T.bw(1) = str2num(docNode.getElementsByTagName('bw_width').item(0).getTextContent);
        T.bw(2) = str2num(docNode.getElementsByTagName('bw_height').item(0).getTextContent);

        T.bw_margin(1) = str2num(docNode.getElementsByTagName('bw_margin_x').item(0).getTextContent);
        T.bw_margin(2) = str2num(docNode.getElementsByTagName('bw_margin_y').item(0).getTextContent);

        xmlList = docNode.getElementsByTagName('Header');

        for i = 0 : xmlList.getLength - 1
            nodeID = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('NodeID').item(0).getTextContent) + 1;
            T.compnames{nodeID} = char(xmlList.item(i).getChildNodes.getElementsByTagName('Name').item(0).getTextContent);
            T.ss(1, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ss_minX').item(0).getTextContent) + 1;
            T.ss(2, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ss_minY').item(0).getTextContent) + 1;
            T.ss(3, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ss_maxX').item(0).getTextContent) + 1;
            T.ss(4, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ss_maxY').item(0).getTextContent) + 1;
            T.components(1, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('win_size_width').item(0).getTextContent);
            T.components(2, nodeID) = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('win_size_height').item(0).getTextContent);
        end;
    
        xmlList = docNode.getElementsByTagName('num_edges');
        xmlEl = xmlList.item(0);
        T.numEdges = str2num(xmlEl.getTextContent);
        
        T.edges = nan(2, T.numEdges);
        
        xmlList = docNode.getElementsByTagName('Edge');
        for i = 0 : xmlList.getLength - 1
            parentID = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ParentID').item(0).getTextContent) + 1;
            childID = str2num(xmlList.item(i).getChildNodes.getElementsByTagName('ChildID').item(0).getTextContent) + 1;
            T.edges(:, i+1) = [parentID; childID];
        end;
        
    end
    
end

