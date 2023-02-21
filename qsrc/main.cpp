#include <CQPetBasicApp.h>
#include <CQPetBasic.h>
#include <CQApp.h>

//#include <QApplication>

int
main(int argc, char **argv)
{
//QApplication qapp(argc, argv);
  CQApp qapp(argc, argv);

  //---

  using FileNames = std::vector<std::string>;

  FileNames fileNames;
  bool      debug = false;
  bool      run   = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "run")
        run = true;
      else if (arg == "debug")
        debug = true;
    }
    else
      fileNames.push_back(argv[i]);
  }

  //---

  auto *app = new CQPetBasicApp;

  auto *basic = app->basic();

  basic->resize(25, 40); // 50 x 40 ?
  basic->setDebug(debug);

  for (const auto &fileName : fileNames)
    basic->loadFile(fileName);

  app->show();

  //---

  if (run)
    basic->run();

  //---

  return qapp.exec();
}
