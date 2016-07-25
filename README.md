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
@article{Uricar-IMAVIS-2016,
  author = {U{\v{r}}i{\v{c}}{\'{a}}{\v{r}}, Michal  and 
            Franc, Vojt{\v{e}}ch and Thomas, Diego and Sugimoto, Akihiro  and Hlav{\'{a}}{\v{c}}, V{\'{a}}clav },
  title = {Multi-view facial landmark detector learned by the Structured Output {SVM}},
  journal = {Image and Vision Computing},
  volume = {47},
  pages = {45--59},
  year = {2016},
  month = {March},
  note = {300-W, the First Automatic Facial Landmark Detection in-the-Wild Challenge},
  issn = {0262-8856},
  doi = {http://dx.doi.org/10.1016/j.imavis.2016.02.004},
  url = {http://www.sciencedirect.com/science/article/pii/S0262885616300105},
  publisher = {Elsevier},
  address = {Amsterdam, Netherlands},
  keywords = {Deformable Part Models, Structured output SVM, Facial landmarks detection },
}

```

Visit http://cmp.felk.cvut.cz/~uricamic/clandmark for further information.
