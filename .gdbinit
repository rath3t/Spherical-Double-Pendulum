python
import os
cwd = os.getcwd()
sys.path.insert(0, cwd + '/src/ThirdParty/eigen3/EigenPrettyPrinters')
from printers import register_eigen_printers
register_eigen_printers (None)
end