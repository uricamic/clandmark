# Using MATLAB learning scripts #

Before running learning script `sv_parallel_learn_bmrmconstr.m`, please make sure that you 

1. Compile the *CLandmark* library and *MATLAB interface*.
2. Compile helper mex-functions, by running `compile_mex.m` in `/clandmark/learning/bmrm/` folder.
3. Provide correct path to *CLandmark root folder* and *300W database images* in all scripts mentioned here.
4. Create the model XML file, by running `createModelFor300W.m` in `/clandmark/learning/sv_parallel/preprocess/` folder.
5. Split database into training and validation part, by running `create_TRN_and_VAL.m` in `/clandmark/learning/sv_parallel/data/` folder.
6. Prepare data for learning, by running `transform_F2learning.m` in `/clandmark/learning/sv_parallel/MAT/` folder. 

Learning is meant to be run on a computer with multiple cores. Learning of C-DPM and F-DPM took around 5 days on a 
computer with 12 cores and memory footprint was around 20 GB. 

Feel free to modify the scripts to detect different sets of landmarks or to use different databases.

## Creating the model XML file ##

Once the lamdba range specified in learning script is fully learned, use the lambda minimizing the validation risk for 
model creation. The XML model file is produced by the following code snippet

```MATLAB
% create instance of flandmark 
flandmark = flandmark_class('./model/SV_init.xml', true);

% assuming W contains the learned weights for optimal lambda
flandmark.setW(W);

% creates model.xml file with learned weights
flandmark.write('model.xml');
```
