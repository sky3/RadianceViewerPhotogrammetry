#ifndef LOAD_BUNDLE_FILE_H_
#define LOAD_BUNDLE_FILE_H_

//OLD without config lib
//std::vector< std::shared_ptr<Camera> > loadBundleFile(std::string image_dir, bool display=true, bool loadImage=true);
//std::vector< std::shared_ptr<Camera> > loadBundleFileHDR(std::string image_dir);
//New with config lib
std::vector< std::shared_ptr<Camera> > loadBundleFile(bool display=true, bool loadImage=true);
std::vector< std::shared_ptr<Camera> > loadBundleFileHDR(bool useAll=false, bool loadImage=true);

//std::vector< std::shared_ptr<Camera> > loadCameraInfXml(bool HDR=false, bool loadImage=true);
#endif