function create_xml_init( fname, M, edges, landmark_names, SS, components, bw, bw_margin, name, sigma )
%CREATE_XML_INIT Summary of this function goes here
%   Detailed explanation goes here    

    if nargin < 10
        sigma = -1;
    end;

    fid = fopen(fname, 'w');

    %%  Header of XML-file
    
    fprintf(fid, '<?xml version="1.0" encoding="utf-8"?>\n');
    fprintf(fid, '<clandmark>\n');
    fprintf(fid, ['\t<name>' name '</name>\n']);
    fprintf(fid, ['\t<version>' datestr(now, 'ddd mmm dd HH:MM:SS YYYY') '</version>\n']);
    fprintf(fid, ['\t<num_nodes>' num2str(M) '</num_nodes>\n']);
    fprintf(fid, ['\t<num_edges>' num2str(M-1) '</num_edges>\n']);
    fprintf(fid, '\t<graph_type>1</graph_type>\n');
    fprintf(fid, ['\t<bw_width>' num2str(bw(1)) '</bw_width>\n']);
    fprintf(fid, ['\t<bw_height>' num2str(bw(2)) '</bw_height>\n']);
    fprintf(fid, ['\t<bw_margin_x>' num2str(bw_margin(1)) '</bw_margin_x>\n']);
    fprintf(fid, ['\t<bw_margin_y>' num2str(bw_margin(2)) '</bw_margin_y>\n']);
    fprintf(fid, ['\t<sigma>' num2str(sigma) '</sigma>\n']);

    %% XML - nodes
    
    fprintf(fid, '\t<Nodes>\n');

    for j = 1 : M

        fprintf(fid, '\t\t<Node>\n');
        fprintf(fid, '\t\t\t<Header>\n');
        fprintf(fid, ['\t\t\t\t<NodeID>' num2str(j-1) '</NodeID>\n']);
        fprintf(fid, ['\t\t\t\t<Name>' landmark_names{j} '</Name>\n']);
        fprintf(fid, ['\t\t\t\t<ss_minX>' num2str(SS(1, j)) '</ss_minX>\n']);
        fprintf(fid, ['\t\t\t\t<ss_minY>' num2str(SS(2, j)) '</ss_minY>\n']);
        fprintf(fid, ['\t\t\t\t<ss_maxX>' num2str(SS(3, j)) '</ss_maxX>\n']);
        fprintf(fid, ['\t\t\t\t<ss_maxY>' num2str(SS(4, j)) '</ss_maxY>\n']);
        fprintf(fid,  '\t\t\t\t<LossType>TABLE_LOSS</LossType>\n');
        fprintf(fid, ['\t\t\t\t<win_size_width>' num2str(components(1, j)) '</win_size_width>\n']);
        fprintf(fid, ['\t\t\t\t<win_size_height>' num2str(components(2, j)) '</win_size_height>\n']);
        fprintf(fid, '\t\t\t</Header>\n');
        fprintf(fid, '\t\t\t<Appearances>\n');
        fprintf(fid, '\t\t\t\t<Appearance>\n');
        fprintf(fid, '\t\t\t\t\t<Type>SPARSE_LBP</Type>\n');
        fprintf(fid, '\t\t\t\t\t<hop>4</hop>\n');
        fprintf(fid, '\t\t\t\t</Appearance>\n');
        fprintf(fid, '\t\t\t</Appearances>\n');
        fprintf(fid, '\t\t</Node>\n');

    end;

    fprintf(fid, '\t</Nodes>\n');

    %% XML - edges
    
    fprintf(fid, '\t<Edges>\n');

    for j = 1 : size(edges, 2);

        fprintf(fid, '\t\t<Edge>\n');
        fprintf(fid, ['\t\t\t<ParentID>' num2str(edges(1, j)) '</ParentID>\n']);
        fprintf(fid, ['\t\t\t<ChildID>' num2str(edges(2, j)) '</ChildID>\n']);
        fprintf(fid, '\t\t\t<Type>1</Type>\n');
        fprintf(fid, '\t\t\t<Dims>4</Dims>\n');
        fprintf(fid, '\t\t</Edge>\n');

    end;

    fprintf(fid, '\t</Edges>\n');
    fprintf(fid, '</clandmark>\n');
    fclose(fid);

end

