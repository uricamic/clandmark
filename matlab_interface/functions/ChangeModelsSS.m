function ChangeModelsSS( fname, newSS, newfname )
%CHANGEMODELSSS Summary of this function goes here
%   Detailed explanation goes here

    fl = flandmark_class(fname, true);
    W = fl.getW();
    clear fl;
    
    T = flandmark_xmlread(fname);
    
    create_xml_init('tmp.xml', T.numNodes, T.edges-1, T.compnames, newSS, T.components, T.bw, T.bw_margin, T.name);

    fl = flandmark_class('tmp.xml', true);
    fl.setW(W);
    fl.write(newfname);
    clear fl;
    
end

