-- =====================================================================
-- 实验3：QtSQL 数据库模块与 View-Model 结构的使用
-- 医院诊疗系统用户信息管理 —— 数据库与数据表创建脚本
-- 数据库：MySQL 8.x，字符集 utf8mb4，存储引擎 InnoDB
-- 说明：依据实验报告 ER 图设计，字段长度已调整为合适大小
-- =====================================================================

-- 如数据库已存在则删除后重建，保证脚本可重复执行
DROP DATABASE IF EXISTS hospital_system;
CREATE DATABASE hospital_system
    DEFAULT CHARACTER SET utf8mb4
    DEFAULT COLLATE utf8mb4_general_ci;

USE hospital_system;
SET NAMES utf8mb4;

-- ---------------------------------------------------------------------
-- 1. 科室表 Department
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS department;
CREATE TABLE department (
    ID          VARCHAR(32)  NOT NULL COMMENT '科室编号（主键）',
    NAME        VARCHAR(50)  NOT NULL COMMENT '科室名称',
    PRIMARY KEY (ID)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '科室信息表';

-- ---------------------------------------------------------------------
-- 2. 用户表 User（登录账号）
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
    ID          VARCHAR(32)  NOT NULL COMMENT '用户编号（主键）',
    FULLNAME    VARCHAR(50)  NOT NULL COMMENT '用户姓名',
    USERNAME    VARCHAR(50)  NOT NULL COMMENT '登录用户名',
    PASSWORD    VARCHAR(50)  NOT NULL COMMENT '登录密码',
    PRIMARY KEY (ID),
    UNIQUE KEY uk_username (USERNAME)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '系统用户表';

-- ---------------------------------------------------------------------
-- 3. 患者表 Patient
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    ID                VARCHAR(32)  NOT NULL COMMENT '患者编号（主键）',
    ID_CARD           VARCHAR(20)  DEFAULT NULL COMMENT '身份证号',
    NAME              VARCHAR(50)  NOT NULL COMMENT '患者姓名',
    SEX               TINYINT      DEFAULT 0  COMMENT '性别：1-男，0-女',
    DOB               VARCHAR(20)  DEFAULT NULL COMMENT '出生日期（yyyy/M/d）',
    HEIGHT            DOUBLE       DEFAULT NULL COMMENT '身高（cm）',
    WEIGHT            DOUBLE       DEFAULT NULL COMMENT '体重（kg）',
    MOBILEPHONE       VARCHAR(20)  DEFAULT NULL COMMENT '手机号码',
    AGE               INT          DEFAULT NULL COMMENT '年龄',
    CREATEDTIMESTAMP  VARCHAR(30)  DEFAULT NULL COMMENT '记录创建时间戳',
    PRIMARY KEY (ID)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '患者信息表';

-- ---------------------------------------------------------------------
-- 4. 医生表 Doctor（关联科室 Department）
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS doctor;
CREATE TABLE doctor (
    ID            VARCHAR(32)  NOT NULL COMMENT '医生编号（主键）',
    EMPLOYEEENO   VARCHAR(32)  NOT NULL COMMENT '工号',
    NAME          VARCHAR(50)  NOT NULL COMMENT '医生姓名',
    DEPARTMENT_ID VARCHAR(32)  DEFAULT NULL COMMENT '所属科室编号',
    PRIMARY KEY (ID),
    KEY idx_dept (DEPARTMENT_ID),
    CONSTRAINT fk_doctor_dept
        FOREIGN KEY (DEPARTMENT_ID) REFERENCES department (ID)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '医生信息表';

-- ---------------------------------------------------------------------
-- 5. 操作记录表 History（关联用户 User）
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS history;
CREATE TABLE history (
    ID        VARCHAR(32)  NOT NULL COMMENT '记录编号（主键）',
    USER_ID   VARCHAR(32)  DEFAULT NULL COMMENT '操作用户编号',
    EVENT     VARCHAR(255) DEFAULT NULL COMMENT '操作内容',
    TIMESTAMP VARCHAR(30)  DEFAULT NULL COMMENT '操作时间戳',
    PRIMARY KEY (ID),
    KEY idx_user (USER_ID),
    CONSTRAINT fk_history_user
        FOREIGN KEY (USER_ID) REFERENCES `user` (ID)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '用户操作记录表';

-- =====================================================================
-- 初始演示数据
-- =====================================================================

-- 默认登录账号：admin / 123456
INSERT INTO `user` (ID, FULLNAME, USERNAME, PASSWORD)
VALUES ('U0001', '系统管理员', 'admin', '123456');

-- 科室演示数据
INSERT INTO department (ID, NAME)
VALUES ('D001', '内科'),
       ('D002', '外科');

-- 医生演示数据
INSERT INTO doctor (ID, EMPLOYEEENO, NAME, DEPARTMENT_ID)
VALUES ('DOC001', 'E1001', '张医生', 'D001'),
       ('DOC002', 'E1002', '李医生', 'D002');

-- 患者演示数据
INSERT INTO patient (ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT, MOBILEPHONE, AGE, CREATEDTIMESTAMP)
VALUES ('P0001', '441900199001011234', '王小明', 1, '1990/1/1', 175.0, 68.5, '13800000001', 36, '2026/11/26 10:00:00'),
       ('P0002', '441900199202022345', '刘小红', 0, '1992/2/2', 162.0, 50.0, '13800000002', 34, '2026/11/26 10:05:00'),
       ('P0003', '441900198803033456', '陈大勇', 1, '1988/3/3', 180.0, 82.0, '13800000003', 38, '2026/11/26 10:10:00');
