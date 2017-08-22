#ifndef CUBBYFLOW_MANUAL_TEST_H
#define CUBBYFLOW_MANUAL_TEST_H

#define LAGACY_CODE 0

#include <Array/ArrayAccessor1.h>
#include <Array/ArrayAccessor2.h>
#include <Array/ArrayAccessor3.h>
#include <Geometry/TriangleMesh3.h>
#include <Utils/Macros.h>

#include <cnpy/cnpy/cnpy.h>
#include <gtest/gtest.h>
#include <pystring/pystring/pystring.h>

#include <fstream>
#include <string>
#include <vector>

#if LAGACY_CODE == 0
#include <experimental/filesystem>
#endif
#ifdef CUBBYFLOW_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define CUBBYFLOW_TEST_OUTPUT_DIR "manual_tests_output"

inline void CreateDirectory(const std::string& dirName)
{
	std::vector<std::string> tokens;
	pystring::split(dirName, tokens, "/");
	std::string partialDir;

#if (LAGACY_CODE == 1)
	for (const auto& token : tokens)
	{
		partialDir = pystring::os::path::join(partialDir, token);
#ifdef CUBBYFLOW_WINDOWS
		_mkdir(partialDir.c_str());
#else
		mkdir(partialDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
	}
#else //LAGACY_CODE == 1
    for (const auto& token : tokens)
    {
        std::experimental::filesystem::create_directory(token);
    }
#endif // LAGACY_CODE != 1
}

#define CUBBYFLOW_TESTS(TestSetName) \
    class TestSetName##Tests : public ::testing::Test \
	{ \
	protected: \
        void SetUp() override \
		{ \
            m_testCollectionDir = pystring::os::path::join(CUBBYFLOW_TEST_OUTPUT_DIR, #TestSetName); \
            CreateDirectory(m_testCollectionDir); \
        } \
		\
        void CreateTestDirectory(const std::string& name) \
		{ \
            m_currentTestDir = GetTestDirectoryName(name); \
            CreateDirectory(m_currentTestDir); \
        } \
		\
        std::string GetFullFilePath(const std::string& name) \
		{ \
            if (!m_currentTestDir.empty()) \
			{ \
                return pystring::os::path::join(m_currentTestDir, name); \
            } \
			else \
			{ \
                return name; \
            } \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor1<T>& data, const std::string& name) \
		{ \
            std::string fileName = GetFullFilePath(name); \
            unsigned int dim[1] = { static_cast<unsigned int>(data.Size()) }; \
            cnpy::npy_save(fileName, data.Data(), dim, 1, "w"); \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor1<T>& data, size_t size, const std::string& name) \
		{ \
            std::string fileName = GetFullFilePath(name); \
            unsigned int dim[1] = { static_cast<unsigned int>(size) }; \
            cnpy::npy_save(fileName, data.Data(), dim, 1, "w"); \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor2<T>& data, const std::string& name) \
		{ \
            std::string fileName = GetFullFilePath(name); \
            unsigned int dim[2] = { \
                static_cast<unsigned int>(data.height()), \
                static_cast<unsigned int>(data.width()) \
            }; \
            cnpy::npy_save(fileName, data.Data(), dim, 2, "w"); \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor2<T>& data, unsigned int frameNum) \
		{ \
            char fileName[256]; \
            snprintf(fileName, sizeof(fileName), "data.#grid2,%04d.npy", frameNum); \
            SaveData(data, fileName); \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor3<T>& data, const std::string& name) \
		{ \
            std::string fileName = GetFullFilePath(name); \
            unsigned int dim[3] = { \
                static_cast<unsigned int>(data.Depth()), \
                static_cast<unsigned int>(data.Height()), \
                static_cast<unsigned int>(data.Width()) \
            }; \
            cnpy::npy_save(fileName, data.Data(), dim, 3, "w"); \
        } \
		\
        template <typename T> \
        void SaveData(const ConstArrayAccessor3<T>& data, unsigned int frameNum) \
		{ \
            char fileName[256]; \
            snprintf(fileName, sizeof(fileName), "data.#grid3,%04d.npy", frameNum); \
            SaveData(data, fileName); \
        } \
		\
        template <typename ParticleSystem> \
        void SaveParticleDataXY(const std::shared_ptr<ParticleSystem>& particles, unsigned int frameNum) \
		{ \
            size_t n = particles->NumberOfParticles(); \
            Array1<double> x(n); \
            Array1<double> y(n); \
            auto positions = particles->Positions(); \
			\
            for (size_t i = 0; i < n; ++i) \
			{ \
                x[i] = positions[i].x; \
                y[i] = positions[i].y; \
            } \
			\
            char fileName[256]; \
            snprintf(fileName, sizeof(fileName), "data.#point2,%04d,x.npy", frameNum); \
            SaveData(x.ConstAccessor(), fileName); \
            snprintf(fileName, sizeof(fileName), "data.#point2,%04d,y.npy", frameNum); \
            SaveData(y.ConstAccessor(), fileName); \
        } \
		\
        void SaveTriangleMeshData(const TriangleMesh3& data, const std::string& name) \
		{ \
            std::string fileName = GetFullFilePath(name); \
            std::ofstream file(fileName.c_str()); \
            if (file) \
			{ \
                data.WriteObj(&file); \
                file.close(); \
            } \
        } \
		\
        std::string GetTestDirectoryName(const std::string& name) \
		{ \
            return pystring::os::path::join(m_testCollectionDir, name); \
        } \
	\
	private: \
		std::string m_testCollectionDir; \
		std::string m_currentTestCaseName; \
		std::string m_currentTestDir; \
    }; \

#define CUBBYFLOW_BEGIN_TEST_F(TestSetName, TestCaseName) \
    TEST_F(TestSetName##Tests, TestCaseName) \
	{ \
        CreateTestDirectory(#TestCaseName);

#define CUBBYFLOW_END_TEST_F }

#endif