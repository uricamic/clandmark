# clandmark

[![Join the chat at https://gitter.im/uricamic/clandmark](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/uricamic/clandmark?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Open Source Landmarking Library

Detailed description will be added soon

## Dependencies

### libclandmark

 - CImg (>= 1.5.6)
 - RapidXML (1.13)

If any of these libraries are installed in a known system prefix, CLandmark will try to use the already installed version.
Otherwise, the internal version will be used and its files will be installed alongside CLandmark.

**CAVEAT**: The version of RapidXML that comes with CLandmark has been changed to fix some missing forward declarations.

## References

In case you use clandmark in an academic work, please cite the following paper:

```tex
@InProceedings{
  author = {U{\v{r}}i{\v{c}}{\'{a}}{\v{r}}, Michal and Franc, Vojt{\v{e}}ch and Thomas, Diego and Sugimoto, Akihiro and Hlav{\'{a}}{\v{c}}, V{\'{a}}clav},
  title = {{Real-time Multi-view Facial Landmark Detector Learned by the Structured Output SVM}},
  year = {2015},
  booktitle = {BWILD '15: Biometrics in the Wild 2015 (IEEE FG 2015 Workshop)},
  venue = {Ljubljana, Slovenia}
  www = {http://luks.fe.uni-lj.si/bwild15},
}
```

Visit http://cmp.felk.cvut.cz/~uricamic/clandmark for further information.
