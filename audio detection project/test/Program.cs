using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace test
{
    class Program
    {
        [DllImport(@"C:\Users\xwy\Documents\Visual Studio 2013\Projects\Audiotranscription\C++ Audio boundary detection\audio detection project\libsndfile-MSVC-master\Release\dll export.dll")]
        public static extern int getlen(byte[] sb);
        static void Main(string[] args)
        {
            StringBuilder sb = new StringBuilder(@"C:\新闻联播\123.wave");
            byte[] bytes = Encoding.Unicode.GetBytes(sb.ToString());
            Encoding gb2312 = Encoding.GetEncoding("gb2312");
            bytes = Encoding.Convert(Encoding.Unicode, gb2312, bytes);
            bytes = gb2312.GetBytes(gb2312.GetString(bytes));
            getlen(bytes);
            return;
        }
    }
}
